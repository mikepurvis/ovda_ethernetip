/**
Software License Agreement (BSD)

\file      connection.h
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

#ifndef ODVA_ETHERNETIP_CONNECTION_H
#define ODVA_ETHERNETIP_CONNECTION_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "odva_ethernetip/eip_types.h"
#include "odva_ethernetip/path.h"
#include "odva_ethernetip/forward_open_request.h"
#include "odva_ethernetip/forward_open_success.h"
#include "odva_ethernetip/forward_close_request.h"
#include "odva_ethernetip/forward_close_success.h"

using boost::shared_ptr;

namespace eip {

/**
 * Class to encapsulate the EIP concept of a connection.
 */
class Connection
{
public:

  EIP_UINT originator_vendor_id;
  EIP_UDINT originator_sn;
  EIP_UINT connection_sn;

  EIP_UDINT o_to_t_connection_id;
  EIP_UDINT t_to_o_connection_id;

  EIP_BYTE timeout_tick_size;
  EIP_USINT timeout_ticks;
  EIP_USINT timeout_multiplyer;

  EIP_UDINT o_to_t_rpi;
  EIP_UDINT t_to_o_rpi;
  EIP_UDINT o_to_t_api;
  EIP_UDINT t_to_o_api;

  EIP_UINT o_to_t_buffer_size;
  EIP_UINT t_to_o_buffer_size;

  /**
   * Create a connection instance using the directional values and reasonable defaults
   * @param o_to_t Originator to Target info
   * @param t_to_o Target to Originator info
   */
  Connection(const EIP_CONNECTION_INFO_T& o_to_t, const EIP_CONNECTION_INFO_T& t_to_o);

  /**
   * Get the path in the given message router request
   * @return reference to the current Path
   */
  Path& getPath()
  {
    return path_;
  }

  /**
   * Shortcut to set the path based on connection points
   * @param origin starting connection point
   * @param target ending connection point
   */
  void setConnectionPoints(EIP_USINT origin, EIP_USINT target);

  /**
   * Create the forward open request from the data in this connection object
   */
  shared_ptr<ForwardOpenRequest> createForwardOpenRequest()
  {
    // Maintains backwards compatability
    return createForwardOpenRequest(false);
  }

  /**
   * Create the forward open request from the data in this connection object
   * @param use_legacy_forward_open_request use 16 bit connection parameters instead of news 32 bit parameters
   */
  shared_ptr<ForwardOpenRequest> createForwardOpenRequest(bool use_legacy_forward_open_request);

  /**
   * Create a forward close request from the data in this connection object
   */
  shared_ptr<ForwardCloseRequest> createForwardCloseRequest();

  /**
   * Verify that the result of a forward open is valid and update accordingly
   * @return false if there are significant errors in the result
   */
  bool verifyForwardOpenResult(const ForwardOpenSuccess& result);

  /**
   * Verify that the result of a forward close is valid and update accordingly
   * @return false if there are signficant errors in the result
   */
  bool verifyForwardCloseResult(const ForwardCloseSuccess& result);

private:
    Path path_;

};

} // namespace eip

#endif  // ODVA_ETHERNETIP_CONNECTION_H
