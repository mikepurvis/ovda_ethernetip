/**
Software License Agreement (BSD)

\file      session.cpp
\authors   Kareem Shehata <kareem@shehata.ca>
\copyright Copyright (c) 2015, Clearpath Robotics, Inc., All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the
   following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
   following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of Clearpath Robotics nor the names of its contributors may be used to endorse or promote
   products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WAR-
RANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, IN-
DIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <console_bridge/console.h>

#include "odva_ethernetip/serialization/buffer_reader.h"
#include "odva_ethernetip/serialization/buffer_writer.h"
#include "odva_ethernetip/session.h"
#include "odva_ethernetip/eip_types.h"
#include "odva_ethernetip/encap_packet.h"
#include "odva_ethernetip/register_session_data.h"
#include "odva_ethernetip/rr_data_request.h"
#include "odva_ethernetip/rr_data_response.h"

using boost::shared_ptr;
using boost::make_shared;
using std::endl;

namespace eip {

using serialization::BufferReader;
using serialization::BufferWriter;

Session::Session(shared_ptr<Socket> socket, shared_ptr<Socket> io_socket,
    EIP_UINT vendor_id, EIP_UDINT serial_num)
    : socket_(socket), io_socket_(io_socket), session_id_(0),
      my_vendor_id_(vendor_id), my_serial_num_(serial_num)
{
  // generate pseudo-random connection ID and connection SN starting points
  boost::random::mt19937 gen;
  gen.seed(std::time(0));
  boost::random::uniform_int_distribution<> dist(0, 0xFFFF);
  next_connection_id_ = gen();
  next_connection_sn_ = dist(gen);
  CONSOLE_BRIDGE_logInform("Generated starting connection ID %zu and SN %zu", next_connection_id_, next_connection_sn_);
}

Session::~Session()
{
  try
  {
    if (session_id_ != 0)
    {
      close();
    }
  }
  catch (...)
  {
    // can't throw exceptions, but can't do anything either
  }
}

void Session::open(string hostname, string port, string io_port)
{
  CONSOLE_BRIDGE_logInform("Resolving hostname and connecting socket");
  socket_->open(hostname, port);
  io_socket_->open(hostname, io_port);

  // create the registration message
  CONSOLE_BRIDGE_logInform("Creating and sending the registration message");
  shared_ptr<RegisterSessionData> reg_data = make_shared<RegisterSessionData>();
  EncapPacket reg_msg(EIP_CMD_REGISTER_SESSION, 0, reg_data);

  // send the register session message and get response
  EncapPacket response;
  try
  {
    response = sendCommand(reg_msg);
  }
  catch (std::length_error ex)
  {
    socket_->close();
    io_socket_->close();
    CONSOLE_BRIDGE_logError("Could not parse response when registering session: %s", ex.what());
    throw std::runtime_error("Invalid response received registering session");
  }
  catch (std::logic_error ex)
  {
    socket_->close();
    io_socket_->close();
    CONSOLE_BRIDGE_logError("Error in registration response: %s", ex.what());
    throw std::runtime_error("Error in registration response");
  }

  if (response.getHeader().length != reg_data->getLength())
  {
    CONSOLE_BRIDGE_logWarn("Registration message received with wrong size. Expected %zu bytes, received %u",
                           reg_data->getLength(), response.getHeader().length);
  }

  bool response_valid = false;
  try
  {
    response.getPayloadAs(*reg_data);
    response_valid = true;
  }
  catch (std::length_error ex)
  {
    CONSOLE_BRIDGE_logWarn("Registration message too short, ignoring");
  }
  catch (std::logic_error ex)
  {
    CONSOLE_BRIDGE_logWarn("Could not parse registration response: %s", ex.what());
  }

  if (response_valid && reg_data->protocol_version != EIP_PROTOCOL_VERSION)
  {
    CONSOLE_BRIDGE_logError("Error: Wrong Ethernet Industrial Protocol Version. Expected %u got %u",
                            EIP_PROTOCOL_VERSION, reg_data->protocol_version);
    socket_->close();
    io_socket_->close();
    throw std::runtime_error("Received wrong Ethernet IP Protocol Version on registration");
  }
  if (response_valid && reg_data->options != 0)
  {
    CONSOLE_BRIDGE_logWarn("Registration message included non-zero options flags: %u", reg_data->options);
  }

  session_id_ = response.getHeader().session_handle;
  CONSOLE_BRIDGE_logInform("Successfully opened session ID %zu", session_id_);
}

void Session::close()
{
  // TODO: should close all connections and the IO port
  CONSOLE_BRIDGE_logInform("Closing session");

  // create the unregister session message
  EncapPacket reg_msg(EIP_CMD_UNREGISTER_SESSION, session_id_);
  socket_->send(reg_msg);

  CONSOLE_BRIDGE_logInform("Session closed");

  socket_->close();
  io_socket_->close();
  session_id_ = 0;
}

EncapPacket Session::sendCommand(EncapPacket& req)
{
  CONSOLE_BRIDGE_logDebug("Sending Command");
  socket_->send(req);

  CONSOLE_BRIDGE_logDebug("Waiting for response");
  size_t n = socket_->receive(buffer(recv_buffer_));
  CONSOLE_BRIDGE_logDebug("Received response of %zu bytes", n);

  BufferReader reader(buffer(recv_buffer_, n));
  EncapPacket result;
  result.deserialize(reader);

  if (reader.getByteCount() != n)
  {
    CONSOLE_BRIDGE_logWarn("Packet received with %zu bytes, but only %zu bytes used", n, reader.getByteCount());
  }

  check_packet(result, req.getHeader().command);
  return result;
}

void Session::check_packet(EncapPacket& pkt, EIP_UINT exp_cmd)
{
  // verify that all fields are correct
  if (pkt.getHeader().command != exp_cmd)
  {
    CONSOLE_BRIDGE_logError("Reply received with wrong command. Expected %u received %u", exp_cmd,
                            pkt.getHeader().command);
    throw std::logic_error("Reply received with wrong command");
  }
  if (session_id_ == 0 && pkt.getHeader().session_handle == 0)
  {
    CONSOLE_BRIDGE_logError("Zero session handle received on registration: %zu", pkt.getHeader().session_handle);
    throw std::logic_error("Zero session handle received on registration");
  }
  if (session_id_ != 0 && pkt.getHeader().session_handle != session_id_)
  {
    CONSOLE_BRIDGE_logError("Reply received with wrong session ID. Expected %zu, received %zu", session_id_,
                            pkt.getHeader().session_handle);
    throw std::logic_error("Wrong session ID received for command");
  }
  if (pkt.getHeader().status != 0)
  {
    CONSOLE_BRIDGE_logWarn("Non-zero status received: %zu", pkt.getHeader().status);
  }
  if (pkt.getHeader().context[0] != 0 || pkt.getHeader().context[1] != 0)
  {
    CONSOLE_BRIDGE_logWarn("Non-zero sender context received: %zu/%zu", pkt.getHeader().context[0],
                           pkt.getHeader().context[1]);
  }
  if (pkt.getHeader().options != 0)
  {
    CONSOLE_BRIDGE_logWarn("Non-zero options received: %zu", pkt.getHeader().options);
  }
}

void Session::getSingleAttributeSerializable(EIP_USINT class_id, EIP_USINT instance_id,
  EIP_USINT attribute_id, Serializable& result)
{  
  shared_ptr<Serializable> no_data;
  RRDataResponse resp_data = sendRRDataCommand(0x0E,
    Path(class_id, instance_id, attribute_id), no_data);

  resp_data.getResponseDataAs(result);
}

void Session::setSingleAttributeSerializable(EIP_USINT class_id,
  EIP_USINT instance_id, EIP_USINT attribute_id, shared_ptr<Serializable> data)
{
  RRDataResponse resp_data = sendRRDataCommand(0x10,
    Path(class_id, instance_id, attribute_id), data);
}

RRDataResponse Session::sendRRDataCommand(EIP_USINT service, const Path& path,
  shared_ptr<Serializable> data)
{
  CONSOLE_BRIDGE_logDebug("Creating RR Data Request");
  shared_ptr<RRDataRequest> req_data =
    make_shared<RRDataRequest> (service, path, data);
  EncapPacket encap_pkt(EIP_CMD_SEND_RR_DATA, session_id_, req_data);

  // send command and get response
  EncapPacket response;
  try
  {
    response = sendCommand(encap_pkt);
  }
  catch (std::length_error ex)
  {
    CONSOLE_BRIDGE_logError("Response packet to RR command too short: %s", ex.what());
    throw std::runtime_error("Packet response to RR Data Command too short");
  }
  catch (std::logic_error ex)
  {
    CONSOLE_BRIDGE_logError("Invalid response to RR command: %s", ex.what());
    throw std::runtime_error("Invalid packet response to RR Data Command");
  }

  RRDataResponse resp_data;
  try
  {
    response.getPayloadAs(resp_data);
  }
  catch (std::length_error ex)
  {
    CONSOLE_BRIDGE_logError("Response data to RR command too short: %s", ex.what());
    throw std::runtime_error("Response data to RR Command too short");
  }
  catch (std::logic_error ex)
  {
    CONSOLE_BRIDGE_logError("Invalid data to RR command: %s", ex.what());
    throw std::runtime_error("Invalid data in response to RR command");
  }

  // check that responses are valid
  if (resp_data.getServiceCode() != (service | 0x80))
  {
    CONSOLE_BRIDGE_logWarn("Wrong service code returned for RR Data command. Expected: %d but received %d",
                           (int)service, (int)resp_data.getServiceCode());
    // throw std::runtime_error("Wrong service code returned for RR Data command");
  }
  if (resp_data.getGeneralStatus())
  {
    CONSOLE_BRIDGE_logError("RR Data Command failed with status %d", (int)resp_data.getGeneralStatus());
    throw std::runtime_error("RR Data Command Failed");
  }
  return resp_data;
}

int Session::createConnection(const EIP_CONNECTION_INFO_T& o_to_t,
  const EIP_CONNECTION_INFO_T& t_to_o)
{
  Connection conn(o_to_t, t_to_o);
  conn.originator_vendor_id = my_vendor_id_;
  conn.originator_sn = my_serial_num_;
  conn.connection_sn = next_connection_sn_++;
  conn.o_to_t_connection_id = next_connection_id_++;
  conn.t_to_o_connection_id = next_connection_id_++;

  shared_ptr<ForwardOpenRequest> req = conn.createForwardOpenRequest();
  RRDataResponse resp_data = sendRRDataCommand(0x5B, Path(0x06, 1), req);
  ForwardOpenSuccess result;
  resp_data.getResponseDataAs(result);
  if (!conn.verifyForwardOpenResult(result))
  {
    CONSOLE_BRIDGE_logError("Received invalid response to forward open request");
    throw std::logic_error("Forward Open Response Invalid");
  }

  connections_.push_back(conn);
  return connections_.size() - 1;
}

void Session::closeConnection(size_t n)
{
  shared_ptr<ForwardCloseRequest> req = connections_[n].createForwardCloseRequest();
  RRDataResponse resp_data = sendRRDataCommand(0x4E, Path(0x06, 1), req);
  ForwardCloseSuccess result;
  resp_data.getResponseDataAs(result);
  if (!connections_[n].verifyForwardCloseResult(result))
  {
    CONSOLE_BRIDGE_logError("Received invalid response to forward close request");
    throw std::logic_error("Forward Close Response Invalid");
  }
  // remove the connection from the list
  connections_.erase(connections_.begin() + n);
}

CPFPacket Session::receiveIOPacket()
{
  CONSOLE_BRIDGE_logDebug("Receiving IO packet");
  size_t n = io_socket_->receive(buffer(recv_buffer_));
  CONSOLE_BRIDGE_logDebug("Received IO of %zu bytes", n);

  BufferReader reader(buffer(recv_buffer_, n));
  CPFPacket result;
  result.deserialize(reader);

  if (reader.getByteCount() != n)
  {
    CONSOLE_BRIDGE_logWarn("IO packet received with %zu bytes, but only %zu bytes used", n, reader.getByteCount());
  }

  return result;
}

void Session::sendIOPacket(CPFPacket& pkt)
{
  CONSOLE_BRIDGE_logDebug("Sending CPF Packet on IO Socket");
  io_socket_->send(pkt);
}


} // namespace eip
