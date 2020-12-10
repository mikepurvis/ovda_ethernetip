/**
Software License Agreement (BSD)

\file      session.h
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

#ifndef ODVA_ETHERNETIP_SESSION_H
#define ODVA_ETHERNETIP_SESSION_H

#include <string>
#include <gtest/gtest_prod.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>

#include "odva_ethernetip/eip_types.h"
#include "odva_ethernetip/socket/socket.h"
#include "odva_ethernetip/serialization/serializable.h"
#include "odva_ethernetip/serialization/serializable_primitive.h"
#include "odva_ethernetip/encap_packet.h"
#include "odva_ethernetip/cpf_packet.h"
#include "odva_ethernetip/rr_data_response.h"
#include "odva_ethernetip/path.h"
#include "odva_ethernetip/connection.h"

using std::string;
using boost::shared_ptr;
using boost::make_shared;

namespace eip {

using serialization::Serializable;
using serialization::SerializablePrimitive;
using socket::Socket;

#define DEFAULT_VENDOR_ID 0x1234
#define DEFAULT_SERIAL_NUM 0x6789

/**
 * Class to handle creating and managing Ethernet/IP Sessions
 */
class Session
{
public:

  /**
   * Construct a session to use the given sockets
   * @param socket Control socket, usually a TCP socket
   * @param io_socket IO Socket, usually a UDP socket directed at the same host
   * @param vendor_id My vendor ID number to be used if different than the default
   * @param serial_num My serial number to use when setting up connections
   */
  Session(shared_ptr<Socket> socket, shared_ptr<Socket> io_socket,
    EIP_UINT vendor_id = DEFAULT_VENDOR_ID, EIP_UDINT serial_num = DEFAULT_SERIAL_NUM);

  virtual ~Session();

  /**
   * Open the session by opening the port and requesting a session.
   * @param hostname hostname of the target
   * @param port port to use if other than the standard
   * @throw std::runtime_error on problem opening the connection
   */
  void open(string hostname, string port = "44818", string io_port = "2222");

  /**
   * Close the session by unregistering the session and then closing the port
   */
  void close();

  /**
   * Close the session without unregistering the session and just closing the port
   */
  void closeWithoutUnregister();

  /**
   * Get the ID number assigned to this session by the target
   * @return session ID number
   */
  EIP_UDINT getSessionID()
  {
    return session_id_;
  }

  /**
   * Get a single attribute from the given class / instance / attribute path
   * @param class_id Class ID for the path to get
   * @param instance_id Instance ID number for the path to get
   * @param attribute_id Attribute ID number for the path to get
   * @param result Serializable that will accept the result
   */
  void getSingleAttributeSerializable(EIP_USINT class_id, EIP_USINT instance_id,
    EIP_USINT attribute_id, Serializable& result);

  /**
   * Shortcut to get a single attribute as a primitive type
   * @param class_id Class ID for the path to get
   * @param instance_id Instance ID number for the path to get
   * @param attribute_id Attribute ID number for the path to get
   * @return Attribute value from target
   */
  template <typename T>
  T getSingleAttribute(EIP_USINT class_id, EIP_USINT instance_id, EIP_USINT attribute_id, T v)
  {
    SerializablePrimitive<T> data;
    getSingleAttributeSerializable(class_id, instance_id, attribute_id, data);
    return data.data;
  }

  /**
   * Set a single attribute from the given class / instance / attribute path
   * @param class_id Class ID for the path to get
   * @param instance_id Instance ID number for the path to get
   * @param attribute_id Attribute ID number for the path to get
   * @param data Data to set the attribute to
   */
  void setSingleAttributeSerializable(EIP_USINT class_id, EIP_USINT instance_id,
    EIP_USINT attribute_id, shared_ptr<Serializable> data);

  /**
   * Shortcut to set a single attribute from a primitive type
   * @param class_id Class ID for the path to get
   * @param instance_id Instance ID number for the path to get
   * @param attribute_id Attribute ID number for the path to get
   * @param v Value to set attribute to
   */
  template <typename T>
  void setSingleAttribute(EIP_USINT class_id, EIP_USINT instance_id,
    EIP_USINT attribute_id, T v)
  {
    shared_ptr< SerializablePrimitive<T> > data =
      make_shared< SerializablePrimitive<T> > (v);
    setSingleAttributeSerializable(class_id, instance_id, attribute_id, data);
  }

  /**
   * Create an Ethernet/IP Connection for sending implicit messages
   * @param o_to_t Origin to target connection info
   * @param t_to_o Target to origin connection info
   */
  int createConnection(const EIP_CONNECTION_INFO_T& o_to_t,
    const EIP_CONNECTION_INFO_T& t_to_o)
  {
    // Maintains backwards compatability
    return createConnection(o_to_t, t_to_o, 0x5B, false);
  }

  /**
   * Create an Ethernet/IP Connection for sending implicit messages
   * @param o_to_t Origin to target connection info
   * @param t_to_o Target to origin connection info
   * @param service Service code to send
   * @param use_legacy_forward_open_request use 16 bit connection parameters instead of news 32 bit parameters
   */
  int createConnection(const EIP_CONNECTION_INFO_T& o_to_t,
    const EIP_CONNECTION_INFO_T& t_to_o,
    EIP_USINT service,
    bool use_legacy_forward_open_request);

  /**
   * Close the given connection number
   * @param n Connection number to close
   */
  void closeConnection(size_t n);

  /**
   * Accessor for the Vendor ID to be used for this session
   * @return Vendor ID that will be sent to the target with any requests
   */
  inline EIP_UINT getVendorID() const
  {
    return my_vendor_id_;
  }

  /**
   * Accessor for serial number that will be used for this session
   * @return Serial number that will be sent to the target with any requests
   */
  inline EIP_UDINT getSerialNum() const
  {
    return my_serial_num_;
  }

  /**
   * Accessor for connection info
   * @param n Connection number to access
   * @return Connection details
   */
  const Connection& getConnection(size_t n)
  {
    return connections_[n];
  }

  /**
   * Receive the next packet from the IO socket.
   * This is a hack. I'd really much prefer to have the Session receive and do
   * callback through the connection object, but there's no time for that now.
   * @param CPFPacket the received packet
   */
  CPFPacket receiveIOPacket();

  /**
   * Send an IO packet. Yet another hack. Should really keep track of sequence
   * numbers and things inside of the session, but again no time for that.
   */
  void sendIOPacket(CPFPacket& pkt);

private:
  FRIEND_TEST(SessionTest, test_create_connection);

  shared_ptr<Socket> socket_;
  shared_ptr<Socket> io_socket_;
  EIP_UDINT session_id_;
  EIP_BYTE recv_buffer_[4*1024];

  EIP_UINT my_vendor_id_;
  EIP_UDINT my_serial_num_;
  EIP_UINT next_connection_sn_;
  EIP_UDINT next_connection_id_;

  vector<Connection> connections_;

  /**
   * Helper to check a returned encapsulation packet based on an expected command.
   * Note that there are some warnings given that do not produce an exception
   * @throws std::logic_error if problems are found with the response
   */
  void check_packet(EncapPacket& pkt, EIP_UINT exp_cmd);

  /**
   * Helper to send a command to the target as encapsulation packet and get
   * the response, also as an encapsulation packet
   * @param req Encapsulation packet to send
   * @return Encapsulation packet received in response
   */
  EncapPacket sendCommand(EncapPacket& req);

  /**
   * Helper to send an RR Data command to the target
   * @param service Service code to send
   * @param class_id Class ID for the path
   * @param instance_id Instance ID number for the path
   * @param attribute_id Attribute ID number for the path
   * @param data Data to include. If null, then no data sent
   * @return RR Data Response
   */
  RRDataResponse sendRRDataCommand(EIP_USINT service, const Path& path,
    shared_ptr<Serializable> data);
};

} // namespace eip

#endif  // ODVA_ETHERNETIP_SESSION_H
