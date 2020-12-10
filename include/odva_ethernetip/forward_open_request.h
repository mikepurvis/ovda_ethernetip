/**
Software License Agreement (BSD)

\file      forward_open_request.h
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

#ifndef ODVA_ETHERNETIP_FORWARD_OPEN_REQUEST_H
#define ODVA_ETHERNETIP_FORWARD_OPEN_REQUEST_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "odva_ethernetip/eip_types.h"
#include "odva_ethernetip/serialization/serializable.h"
#include "odva_ethernetip/serialization/reader.h"
#include "odva_ethernetip/serialization/writer.h"
#include "odva_ethernetip/path.h"

using boost::shared_ptr;

namespace eip {

using serialization::Serializable;
using serialization::Reader;
using serialization::Writer;

/**
 * Class to encapsulate a ForwardOpenRequest data. Note that this is currently
 * only LARGE forward open, but could be easily changed to support both.
 */
class ForwardOpenRequest : public Serializable
{
public:

  EIP_BYTE timeout_tick_size;
  EIP_USINT timeout_ticks;
  EIP_UDINT o_to_t_connection_id;
  EIP_UDINT t_to_o_connection_id;
  EIP_UINT connection_sn;
  EIP_UINT originator_vendor_id;
  EIP_UDINT originator_sn;
  EIP_USINT timeout_multiplyer;
  EIP_UDINT o_to_t_rpi;
  EIP_DWORD o_to_t_conn_params;
  EIP_WORD o_to_t_conn_params_legacy;
  EIP_UDINT t_to_o_rpi;
  EIP_DWORD t_to_o_conn_params;
  EIP_WORD t_to_o_conn_params_legacy;
  EIP_BYTE conn_type;

  bool use_legacy_forward_open_request;

  ForwardOpenRequest()
    : use_legacy_forward_open_request(false)
  {
  }

  ForwardOpenRequest(bool use_legacy_forward_open_request)
    : use_legacy_forward_open_request(use_legacy_forward_open_request)
  {
  }

  /**
   * Helper to calculate connection parameters for current 32 bit connection parameters
   * @param size Maximum size of the messages in the connection in byte
   * @param variable if set to true, variable message sizes
   * @param priority Priority value for the connection
   * @param type Connection type / class info
   * @param shared If set to true, then a shared connection
   */
  static EIP_DWORD calcConnectionParams(EIP_UINT size, bool variable, EIP_BYTE priority,
    EIP_BYTE type, bool shared)
  {
    return (size & 0x7FFF) | (variable ? 0x2000000 : 0) | (priority & 0x03) << 26
      | (type & 0x03) << 29 | (shared ? 0x80000000 : 0);
  }

  /**
   * Helper to calculate connection parameters for legacy 16 bit connection parameters
   * @param size Maximum size of the messages in the connection in byte
   * @param variable if set to true, variable message sizes
   * @param priority Priority value for the connection
   * @param type Connection type / class info
   * @param shared If set to true, then a shared connection
   */
  static EIP_WORD calcConnectionParamsLegacy(EIP_UINT size, bool variable, EIP_BYTE priority,
    EIP_BYTE type, bool shared)
  {
    return (size & 0x1FF) | (variable ? 0x200 : 0) | (priority & 0x03) << 10
      | (type & 0x03) << 13 | (shared ? 0x8000 : 0);
  }

  /**
   * Shortcut to set the origin to target parameters.
   */
  EIP_DWORD setOriginToTargetParams(EIP_UINT size, bool variable, EIP_BYTE priority,
    EIP_BYTE type, bool shared)
  {
    if (use_legacy_forward_open_request)
    {
      o_to_t_conn_params_legacy = calcConnectionParamsLegacy(size, variable, priority, type, shared);
    }
    else
    {
      o_to_t_conn_params = calcConnectionParams(size, variable, priority, type, shared);
    }
    return 0;
  }

  /**
   * Shortcut to set the target to origin params.
   */
  EIP_DWORD setTargetToOriginParams(EIP_UINT size, bool variable, EIP_BYTE priority,
    EIP_BYTE type, bool shared)
  {
    if (use_legacy_forward_open_request)
    {
      t_to_o_conn_params_legacy = calcConnectionParamsLegacy(size, variable, priority, type, shared);
    }
    else
    {
      t_to_o_conn_params = calcConnectionParams(size, variable, priority, type, shared);
    }
    return 0;
  }

  /**
   * Get the path in the given message router request
   * @return reference to the current Path
   */
  Path& getPath()
  {
    return path_;
  }

  /**
   * Get the length of serialized data that would be produced if serialized
   * @return Total length in bytes to be serialized
   */
  virtual size_t getLength() const
  {
    size_t ret = sizeof(timeout_tick_size);
    ret += sizeof(timeout_ticks);
    ret += sizeof(o_to_t_connection_id);
    ret += sizeof(t_to_o_connection_id);
    ret += sizeof(connection_sn);
    ret += sizeof(originator_vendor_id);
    ret += sizeof(originator_sn);
    ret += sizeof(timeout_multiplyer);
    ret += sizeof(o_to_t_rpi);
    if (use_legacy_forward_open_request)
    {
      ret += sizeof(o_to_t_conn_params_legacy);
    }
    else
    {
      ret += sizeof(o_to_t_conn_params);
    }
    ret += sizeof(t_to_o_rpi);
    if (use_legacy_forward_open_request)
    {
      ret += sizeof(t_to_o_conn_params_legacy);
    }
    else
    {
      ret += sizeof(t_to_o_conn_params);
    }
    ret += sizeof(conn_type);
    ret += 3; // reserved bytes
    ret += path_.getLength();
    return ret;
  }

  /**
   * Serialize data into the given buffer
   * @param writer Writer to use for serialization
   * @return the writer again
   * @throw std::length_error if the buffer is too small for the header data
   */
  virtual Writer& serialize(Writer& writer) const
  {
    EIP_BYTE reserved = 0;
    writer.write(timeout_tick_size);
    writer.write(timeout_ticks);
    writer.write(o_to_t_connection_id);
    writer.write(t_to_o_connection_id);
    writer.write(connection_sn);
    writer.write(originator_vendor_id);
    writer.write(originator_sn);
    writer.write(timeout_multiplyer);
    writer.write(reserved);
    writer.write(reserved);
    writer.write(reserved);
    writer.write(o_to_t_rpi);
    if (use_legacy_forward_open_request)
    {
      writer.write(o_to_t_conn_params_legacy);
    }
    else
    {
      writer.write(o_to_t_conn_params);
    }
    writer.write(t_to_o_rpi);
    if (use_legacy_forward_open_request)
    {
      writer.write(t_to_o_conn_params_legacy);
    }
    else
    {
      writer.write(t_to_o_conn_params);
    }
    writer.write(conn_type);
    path_.serialize(writer);
    return writer;
  }

  /**
   * Not implemented. Never expect to have to receive this type of message.
   */
  virtual Reader& deserialize(Reader& reader, size_t length)
  {
    throw std::logic_error("Not implemented");
  }

  /**
   * Not implemented. Never expect to have to receive this type of message.
   */
  virtual Reader& deserialize(Reader& reader)
  {
    throw std::logic_error("Not implemented");
  }

private:
  Path path_;
};

} // namespace eip

#endif  // ODVA_ETHERNETIP_FORWARD_OPEN_REQUEST_H
