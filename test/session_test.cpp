/**
Software License Agreement (BSD)

\file      session_test.cpp
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

#include <gtest/gtest.h>
#include <boost/make_shared.hpp>

#include "odva_ethernetip/session.h"
#include "odva_ethernetip/socket/test_socket.h"
#include "odva_ethernetip/rr_data_response.h"
#include "odva_ethernetip/sequenced_address_item.h"
#include "odva_ethernetip/sequenced_data_item.h"
#include "odva_ethernetip/serialization/serializable_buffer.h"
#include "odva_ethernetip/serialization/serializable_primitive.h"
//#include "os32c/measurement_report.h"

using boost::make_shared;
using namespace boost::asio;

using namespace eip::socket;
using namespace eip::serialization;

//using os32c::MeasurementReport;

namespace eip {

class SessionTest : public :: testing :: Test
{
public:
  SessionTest() : session(ts, ts_io) { }

protected:
  virtual void SetUp()
  {
    ts = make_shared<TestSocket> ();
    ts_io = make_shared<TestSocket> ();
    session = Session(ts, ts_io);
  }

  shared_ptr<TestSocket> ts;
  shared_ptr<TestSocket> ts_io;
  Session session;
};

TEST_F(SessionTest, test_open)
{
  unsigned char resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    4, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // protocol version
    1, 0,
    // more options flags
    0, 0,
  };

  ts->rx_buffer = buffer(resp_packet);
  session.open("example_host");
  EXPECT_TRUE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);

  // check the registration packet
  EXPECT_EQ(28, ts->tx_count);
  // command
  EXPECT_EQ(0x65, ts->tx_buffer[0]);
  EXPECT_EQ(   0, ts->tx_buffer[1]);
  // length
  EXPECT_EQ(0x04, ts->tx_buffer[2]);
  EXPECT_EQ(   0, ts->tx_buffer[3]);
  // session handle
  EXPECT_EQ(   0, ts->tx_buffer[4]);
  EXPECT_EQ(   0, ts->tx_buffer[5]);
  EXPECT_EQ(   0, ts->tx_buffer[6]);
  EXPECT_EQ(   0, ts->tx_buffer[7]);
  // status
  EXPECT_EQ(   0, ts->tx_buffer[8]);
  EXPECT_EQ(   0, ts->tx_buffer[9]);
  EXPECT_EQ(   0, ts->tx_buffer[10]);
  EXPECT_EQ(   0, ts->tx_buffer[11]);
  // sender context
  EXPECT_EQ(   0, ts->tx_buffer[12]);
  EXPECT_EQ(   0, ts->tx_buffer[13]);
  EXPECT_EQ(   0, ts->tx_buffer[14]);
  EXPECT_EQ(   0, ts->tx_buffer[15]);
  EXPECT_EQ(   0, ts->tx_buffer[16]);
  EXPECT_EQ(   0, ts->tx_buffer[17]);
  EXPECT_EQ(   0, ts->tx_buffer[18]);
  EXPECT_EQ(   0, ts->tx_buffer[19]);
  // options
  EXPECT_EQ(   0, ts->tx_buffer[20]);
  EXPECT_EQ(   0, ts->tx_buffer[21]);
  EXPECT_EQ(   0, ts->tx_buffer[22]);
  EXPECT_EQ(   0, ts->tx_buffer[23]);
  // protocol version
  EXPECT_EQ(   1, ts->tx_buffer[24]);
  EXPECT_EQ(   0, ts->tx_buffer[25]);
  // more options flags
  EXPECT_EQ(   0, ts->tx_buffer[26]);
  EXPECT_EQ(   0, ts->tx_buffer[27]);

  // check that full packet used
  EXPECT_EQ(sizeof(resp_packet), ts->rx_count);

  EXPECT_EQ(0xDEADBEEF, session.getSessionID());

  // next test that close works correctly
  ts->tx_count = 0;
  memset(ts->tx_buffer, 0, sizeof(ts->tx_count));

  session.close();
  EXPECT_FALSE(ts->is_open);
  EXPECT_EQ(0, session.getSessionID());

  // check the unregistration packet
  EXPECT_EQ(24, ts->tx_count);
  // command
  EXPECT_EQ(0x66, ts->tx_buffer[0]);
  EXPECT_EQ(   0, ts->tx_buffer[1]);
  // length
  EXPECT_EQ(   0, ts->tx_buffer[2]);
  EXPECT_EQ(   0, ts->tx_buffer[3]);
  // session handle
  EXPECT_EQ(0xEF, ts->tx_buffer[4]);
  EXPECT_EQ(0xBE, ts->tx_buffer[5]);
  EXPECT_EQ(0xAD, ts->tx_buffer[6]);
  EXPECT_EQ(0xDE, ts->tx_buffer[7]);
  // status
  EXPECT_EQ(   0, ts->tx_buffer[8]);
  EXPECT_EQ(   0, ts->tx_buffer[9]);
  EXPECT_EQ(   0, ts->tx_buffer[10]);
  EXPECT_EQ(   0, ts->tx_buffer[11]);
  // sender context
  EXPECT_EQ(   0, ts->tx_buffer[12]);
  EXPECT_EQ(   0, ts->tx_buffer[13]);
  EXPECT_EQ(   0, ts->tx_buffer[14]);
  EXPECT_EQ(   0, ts->tx_buffer[15]);
  EXPECT_EQ(   0, ts->tx_buffer[16]);
  EXPECT_EQ(   0, ts->tx_buffer[17]);
  EXPECT_EQ(   0, ts->tx_buffer[18]);
  EXPECT_EQ(   0, ts->tx_buffer[19]);
  // options
  EXPECT_EQ(   0, ts->tx_buffer[20]);
  EXPECT_EQ(   0, ts->tx_buffer[21]);
  EXPECT_EQ(   0, ts->tx_buffer[22]);
  EXPECT_EQ(   0, ts->tx_buffer[23]);
}

TEST_F(SessionTest, test_open_wrong_command_id)
{
  char resp_packet[] = {
    // command for register session
    0x00, 0x00,
    // length
    0, 0,
    // session handle
    0, 0, 0, 0,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
  };

  ts->rx_buffer = buffer(resp_packet);
  EXPECT_THROW(session.open("example_host"), std::runtime_error);
  EXPECT_FALSE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0, session.getSessionID());
}

TEST_F(SessionTest, test_open_zero_session)
{
  char resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    0, 0,
    // session handle
    0, 0, 0, 0,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
  };

  ts->rx_buffer = buffer(resp_packet);
  EXPECT_THROW(session.open("example_host"), std::runtime_error);
  EXPECT_FALSE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0, session.getSessionID());
}

TEST_F(SessionTest, test_open_short_packet)
{
  char resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    1, 0,
    // session handle
    1, 0, 0, 0,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
  };

  ts->rx_buffer = buffer(resp_packet);
  EXPECT_THROW(session.open("example_host"), std::runtime_error);
  EXPECT_FALSE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0, session.getSessionID());
}

TEST_F(SessionTest, test_open_short_data)
{
  char resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    2, 0,
    // session handle
    1, 0, 0, 0,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // version (short)
    1, 0,
  };

  ts->rx_buffer = buffer(resp_packet);
  session.open("example_host");
  EXPECT_TRUE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(1, session.getSessionID());
}

TEST_F(SessionTest, test_open_extra_data)
{
  unsigned char resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    8, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0xFF, 0xFF, 0xFF, 0xFF,
    // sender context
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // options
    0xFF, 0xFF, 0xFF, 0xFF,
    // protocol version
    1, 0,
    // more options flags
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  };

  ts->rx_buffer = buffer(resp_packet);
  session.open("example_host");
  EXPECT_TRUE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0xDEADBEEF, session.getSessionID());
}

TEST_F(SessionTest, test_open_wrong_version)
{
  unsigned char resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    4, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // protocol version
    2, 1,
    // more options flags
    0, 0,
  };

  ts->rx_buffer = buffer(resp_packet);
  EXPECT_THROW(session.open("example_host"), std::runtime_error);
  EXPECT_FALSE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0, session.getSessionID());
}


TEST_F(SessionTest, test_get_single_attribute)
{
  unsigned char reg_resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    4, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // protocol version
    1, 0,
    // more options flags
    0, 0,
  };

  ts->rx_buffer = buffer(reg_resp_packet);
  session.open("example_host");
  EXPECT_TRUE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0xDEADBEEF, session.getSessionID());

  // clear out the tx buffer
  ts->tx_count = 0;
  memset(ts->tx_buffer, 0, sizeof(ts->tx_count));

  unsigned char resp_packet[] = {
    // command for register session
    0x6F, 0x00,
    // length
    24, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // interface handle
    0, 0, 0, 0,
    // timeout
    0, 0,
    // item count
    2, 0,
    // address type
    0, 0,
    // address length
    0, 0,
    // data type
    0xB2, 0x00,
    // data length
    0x08, 0x00,
    // service code
    0x8E,
    // reserved
    0,
    // general status
    0x00,
    // size of additional status
    0,
    // response data
    0xEF, 0xCD, 0xAB, 0xAA,
  };

  ts->rx_buffer = buffer(resp_packet);

  EIP_UDINT result = session.getSingleAttribute(0x75, 1, 3, result);

  // check the unregistration packet
  EXPECT_EQ(48, ts->tx_count);
  // command
  EXPECT_EQ(0x6F, ts->tx_buffer[0]);
  EXPECT_EQ(   0, ts->tx_buffer[1]);
  // length
  EXPECT_EQ(  24, ts->tx_buffer[2]);
  EXPECT_EQ(   0, ts->tx_buffer[3]);
  // session handle
  EXPECT_EQ(0xEF, ts->tx_buffer[4]);
  EXPECT_EQ(0xBE, ts->tx_buffer[5]);
  EXPECT_EQ(0xAD, ts->tx_buffer[6]);
  EXPECT_EQ(0xDE, ts->tx_buffer[7]);
  // status
  EXPECT_EQ(   0, ts->tx_buffer[8]);
  EXPECT_EQ(   0, ts->tx_buffer[9]);
  EXPECT_EQ(   0, ts->tx_buffer[10]);
  EXPECT_EQ(   0, ts->tx_buffer[11]);
  // sender context
  EXPECT_EQ(   0, ts->tx_buffer[12]);
  EXPECT_EQ(   0, ts->tx_buffer[13]);
  EXPECT_EQ(   0, ts->tx_buffer[14]);
  EXPECT_EQ(   0, ts->tx_buffer[15]);
  EXPECT_EQ(   0, ts->tx_buffer[16]);
  EXPECT_EQ(   0, ts->tx_buffer[17]);
  EXPECT_EQ(   0, ts->tx_buffer[18]);
  EXPECT_EQ(   0, ts->tx_buffer[19]);
  // options
  EXPECT_EQ(   0, ts->tx_buffer[20]);
  EXPECT_EQ(   0, ts->tx_buffer[21]);
  EXPECT_EQ(   0, ts->tx_buffer[22]);
  EXPECT_EQ(   0, ts->tx_buffer[23]);
  // interface handle
  EXPECT_EQ(   0, ts->tx_buffer[24]);
  EXPECT_EQ(   0, ts->tx_buffer[25]);
  EXPECT_EQ(   0, ts->tx_buffer[26]);
  EXPECT_EQ(   0, ts->tx_buffer[27]);
  // timeout
  EXPECT_EQ(   0, ts->tx_buffer[28]);
  EXPECT_EQ(   0, ts->tx_buffer[29]);
  // item count
  EXPECT_EQ(   2, ts->tx_buffer[30]);
  EXPECT_EQ(   0, ts->tx_buffer[31]);
  // address type null
  EXPECT_EQ(   0, ts->tx_buffer[32]);
  EXPECT_EQ(   0, ts->tx_buffer[33]);
  // address length 0
  EXPECT_EQ(   0, ts->tx_buffer[34]);
  EXPECT_EQ(   0, ts->tx_buffer[35]);
  // data type for UCMM
  EXPECT_EQ(0xB2, ts->tx_buffer[36]);
  EXPECT_EQ(0x00, ts->tx_buffer[37]);
  // data length
  EXPECT_EQ(   8, ts->tx_buffer[38]);
  EXPECT_EQ(   0, ts->tx_buffer[39]);
  // service code
  EXPECT_EQ(0xE, ts->tx_buffer[40]);
  // path
  EXPECT_EQ(0x03, ts->tx_buffer[41]);
  EXPECT_EQ(0x20, ts->tx_buffer[42]);
  EXPECT_EQ(0x75, ts->tx_buffer[43]);
  EXPECT_EQ(0x24, ts->tx_buffer[44]);
  EXPECT_EQ(0x01, ts->tx_buffer[45]);
  EXPECT_EQ(0x30, ts->tx_buffer[46]);
  EXPECT_EQ(0x03, ts->tx_buffer[47]);

  EXPECT_EQ(0xAAABCDEF, result);
}


TEST_F(SessionTest, test_set_single_attribute)
{
  unsigned char reg_resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    4, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // protocol version
    1, 0,
    // more options flags
    0, 0,
  };

  ts->rx_buffer = buffer(reg_resp_packet);
  session.open("example_host");
  EXPECT_TRUE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0xDEADBEEF, session.getSessionID());

  // clear out the tx buffer
  ts->tx_count = 0;
  memset(ts->tx_buffer, 0, sizeof(ts->tx_count));

  unsigned char resp_packet[] = {
    // command for RR Data
    0x6F, 0x00,
    // length
    20, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // interface handle
    0, 0, 0, 0,
    // timeout
    0, 0,
    // item count
    2, 0,
    // address type
    0, 0,
    // address length
    0, 0,
    // data type
    0xB2, 0x00,
    // data length
    0x04, 0x00,
    // service code
    0x90,
    // reserved
    0,
    // general status
    0x00,
    // size of additional status
    0,
  };

  ts->rx_buffer = buffer(resp_packet);

  session.setSingleAttribute(0x73, 1, 4, (EIP_UINT)1);

  // check the unregistration packet
  EXPECT_EQ(50, ts->tx_count);
  // command
  EXPECT_EQ(0x6F, ts->tx_buffer[0]);
  EXPECT_EQ(   0, ts->tx_buffer[1]);
  // length
  EXPECT_EQ(  26, ts->tx_buffer[2]);
  EXPECT_EQ(   0, ts->tx_buffer[3]);
  // session handle
  EXPECT_EQ(0xEF, ts->tx_buffer[4]);
  EXPECT_EQ(0xBE, ts->tx_buffer[5]);
  EXPECT_EQ(0xAD, ts->tx_buffer[6]);
  EXPECT_EQ(0xDE, ts->tx_buffer[7]);
  // status
  EXPECT_EQ(   0, ts->tx_buffer[8]);
  EXPECT_EQ(   0, ts->tx_buffer[9]);
  EXPECT_EQ(   0, ts->tx_buffer[10]);
  EXPECT_EQ(   0, ts->tx_buffer[11]);
  // sender context
  EXPECT_EQ(   0, ts->tx_buffer[12]);
  EXPECT_EQ(   0, ts->tx_buffer[13]);
  EXPECT_EQ(   0, ts->tx_buffer[14]);
  EXPECT_EQ(   0, ts->tx_buffer[15]);
  EXPECT_EQ(   0, ts->tx_buffer[16]);
  EXPECT_EQ(   0, ts->tx_buffer[17]);
  EXPECT_EQ(   0, ts->tx_buffer[18]);
  EXPECT_EQ(   0, ts->tx_buffer[19]);
  // options
  EXPECT_EQ(   0, ts->tx_buffer[20]);
  EXPECT_EQ(   0, ts->tx_buffer[21]);
  EXPECT_EQ(   0, ts->tx_buffer[22]);
  EXPECT_EQ(   0, ts->tx_buffer[23]);
  // interface handle
  EXPECT_EQ(   0, ts->tx_buffer[24]);
  EXPECT_EQ(   0, ts->tx_buffer[25]);
  EXPECT_EQ(   0, ts->tx_buffer[26]);
  EXPECT_EQ(   0, ts->tx_buffer[27]);
  // timeout
  EXPECT_EQ(   0, ts->tx_buffer[28]);
  EXPECT_EQ(   0, ts->tx_buffer[29]);
  // item count
  EXPECT_EQ(   2, ts->tx_buffer[30]);
  EXPECT_EQ(   0, ts->tx_buffer[31]);
  // address type null
  EXPECT_EQ(   0, ts->tx_buffer[32]);
  EXPECT_EQ(   0, ts->tx_buffer[33]);
  // address length 0
  EXPECT_EQ(   0, ts->tx_buffer[34]);
  EXPECT_EQ(   0, ts->tx_buffer[35]);
  // data type for UCMM
  EXPECT_EQ(0xB2, ts->tx_buffer[36]);
  EXPECT_EQ(0x00, ts->tx_buffer[37]);
  // data length
  EXPECT_EQ(  10, ts->tx_buffer[38]);
  EXPECT_EQ(   0, ts->tx_buffer[39]);
  // service code
  EXPECT_EQ(0x10, ts->tx_buffer[40]);
  // path
  EXPECT_EQ(0x03, ts->tx_buffer[41]);
  EXPECT_EQ(0x20, ts->tx_buffer[42]);
  EXPECT_EQ(0x73, ts->tx_buffer[43]);
  EXPECT_EQ(0x24, ts->tx_buffer[44]);
  EXPECT_EQ(0x01, ts->tx_buffer[45]);
  EXPECT_EQ(0x30, ts->tx_buffer[46]);
  EXPECT_EQ(0x04, ts->tx_buffer[47]);
  // data itself
  EXPECT_EQ(0x01, ts->tx_buffer[48]);
  EXPECT_EQ(0x00, ts->tx_buffer[49]);
}


TEST_F(SessionTest, test_create_connection)
{
  unsigned char reg_resp_packet[] = {
    // command for register session
    0x65, 0x00,
    // length
    4, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // protocol version
    1, 0,
    // more options flags
    0, 0,
  };

  ts->rx_buffer = buffer(reg_resp_packet);
  session.open("example_host");
  EXPECT_TRUE(ts->is_open);
  EXPECT_EQ("example_host", ts->hostname);
  EXPECT_EQ("44818", ts->port);
  EXPECT_EQ(0xDEADBEEF, session.getSessionID());

  // clear out the tx buffer
  ts->tx_count = 0;
  memset(ts->tx_buffer, 0, sizeof(ts->tx_count));

  unsigned char resp_packet[] = {
    // command for RR Data
    0x6F, 0x00,
    // length
    0x56, 0,
    // session handle
    0xEF, 0xBE, 0xAD, 0xDE,
    // status
    0, 0, 0, 0,
    // sender context. Should probably mirror, but this will always be 0
    0, 0, 0, 0, 0, 0, 0, 0,
    // options
    0, 0, 0, 0,
    // interface handle
    0, 0, 0, 0,
    // timeout
    0, 0,
    // item count
    2, 0,
    // address type
    0, 0,
    // address length
    0, 0,
    // data type
    0xB2, 0x00,
    // data length
    0x1E, 0x00,
    // service code
    0xDB,
    // reserved
    0,
    // general status
    0x00,
    // size of additional status
    0,
    // response data
    0x02, 0x00, 0x0C, 0x78, 0x04, 0x00, 0x02, 0x00,
    0x89, 0x67, 0x95, 0x01, 0x21, 0x43, 0x00, 0x00,
    0xD0, 0x81, 0x17, 0x00, 0xD8, 0x5C, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x10, 0x00, 0x00, 0x02,
    0x08, 0xAE, 0xC0, 0xA8, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80,
    0x10, 0x00, 0x00, 0x02, 0x08, 0xAE, 0xC0, 0xA8,
    0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
  };

  EXPECT_EQ(0x56 + 24, sizeof(resp_packet));
  ts->rx_buffer = buffer(resp_packet);

  EIP_CONNECTION_INFO_T o_to_t, t_to_o;
  o_to_t.assembly_id = 0x71;
  o_to_t.buffer_size = 0x006E;
  o_to_t.rpi = 0x00177FA0;
  t_to_o.assembly_id = 0x66;
  t_to_o.buffer_size = 0x0584;
  t_to_o.rpi = 0x00013070;

  session.my_vendor_id_ = 0x0195;
  session.my_serial_num_ = 0x00004321;
  session.next_connection_sn_ = 0x6789;
  session.next_connection_id_ = 0x00020003;

  int conn_num = session.createConnection(o_to_t, t_to_o);

  // check the forward open packet
  EXPECT_EQ(94, ts->tx_count);
  // command
  EXPECT_EQ(0x6F, ts->tx_buffer[0]);
  EXPECT_EQ(   0, ts->tx_buffer[1]);
  // length
  EXPECT_EQ(0x46, ts->tx_buffer[2]);
  EXPECT_EQ(   0, ts->tx_buffer[3]);
  // session handle
  EXPECT_EQ(0xEF, ts->tx_buffer[4]);
  EXPECT_EQ(0xBE, ts->tx_buffer[5]);
  EXPECT_EQ(0xAD, ts->tx_buffer[6]);
  EXPECT_EQ(0xDE, ts->tx_buffer[7]);
  // status
  EXPECT_EQ(   0, ts->tx_buffer[8]);
  EXPECT_EQ(   0, ts->tx_buffer[9]);
  EXPECT_EQ(   0, ts->tx_buffer[10]);
  EXPECT_EQ(   0, ts->tx_buffer[11]);
  // sender context
  EXPECT_EQ(   0, ts->tx_buffer[12]);
  EXPECT_EQ(   0, ts->tx_buffer[13]);
  EXPECT_EQ(   0, ts->tx_buffer[14]);
  EXPECT_EQ(   0, ts->tx_buffer[15]);
  EXPECT_EQ(   0, ts->tx_buffer[16]);
  EXPECT_EQ(   0, ts->tx_buffer[17]);
  EXPECT_EQ(   0, ts->tx_buffer[18]);
  EXPECT_EQ(   0, ts->tx_buffer[19]);
  // options
  EXPECT_EQ(   0, ts->tx_buffer[20]);
  EXPECT_EQ(   0, ts->tx_buffer[21]);
  EXPECT_EQ(   0, ts->tx_buffer[22]);
  EXPECT_EQ(   0, ts->tx_buffer[23]);
  // interface handle
  EXPECT_EQ(   0, ts->tx_buffer[24]);
  EXPECT_EQ(   0, ts->tx_buffer[25]);
  EXPECT_EQ(   0, ts->tx_buffer[26]);
  EXPECT_EQ(   0, ts->tx_buffer[27]);
  // timeout
  EXPECT_EQ(   0, ts->tx_buffer[28]);
  EXPECT_EQ(   0, ts->tx_buffer[29]);
  // item count
  EXPECT_EQ(   2, ts->tx_buffer[30]);
  EXPECT_EQ(   0, ts->tx_buffer[31]);
  // address type null
  EXPECT_EQ(   0, ts->tx_buffer[32]);
  EXPECT_EQ(   0, ts->tx_buffer[33]);
  // address length 0
  EXPECT_EQ(   0, ts->tx_buffer[34]);
  EXPECT_EQ(   0, ts->tx_buffer[35]);
  // data type for UCMM
  EXPECT_EQ(0xB2, ts->tx_buffer[36]);
  EXPECT_EQ(0x00, ts->tx_buffer[37]);
  // data length
  EXPECT_EQ(0x36, ts->tx_buffer[38]);
  EXPECT_EQ(   0, ts->tx_buffer[39]);
  // service code
  EXPECT_EQ(0x5B, ts->tx_buffer[40]);
  // path
  EXPECT_EQ(0x02, ts->tx_buffer[41]);
  EXPECT_EQ(0x20, ts->tx_buffer[42]);
  EXPECT_EQ(0x06, ts->tx_buffer[43]);
  EXPECT_EQ(0x24, ts->tx_buffer[44]);
  EXPECT_EQ(0x01, ts->tx_buffer[45]);
  // forward open request
  EXPECT_EQ(0x06, ts->tx_buffer[46]);
  EXPECT_EQ(0x50, ts->tx_buffer[47]);
  EXPECT_EQ(0x03, ts->tx_buffer[48]);
  EXPECT_EQ(0x00, ts->tx_buffer[49]);
  EXPECT_EQ(0x02, ts->tx_buffer[50]);
  EXPECT_EQ(0x00, ts->tx_buffer[51]);
  EXPECT_EQ(0x04, ts->tx_buffer[52]);
  EXPECT_EQ(0x00, ts->tx_buffer[53]);
  EXPECT_EQ(0x02, ts->tx_buffer[54]);
  EXPECT_EQ(0x00, ts->tx_buffer[55]);
  EXPECT_EQ(0x89, ts->tx_buffer[56]);
  EXPECT_EQ(0x67, ts->tx_buffer[57]);
  EXPECT_EQ(0x95, ts->tx_buffer[58]);
  EXPECT_EQ(0x01, ts->tx_buffer[59]);
  EXPECT_EQ(0x21, ts->tx_buffer[60]);
  EXPECT_EQ(0x43, ts->tx_buffer[61]);
  EXPECT_EQ(0x00, ts->tx_buffer[62]);
  EXPECT_EQ(0x00, ts->tx_buffer[63]);
  EXPECT_EQ(0x00, ts->tx_buffer[64]);
  EXPECT_EQ(0x00, ts->tx_buffer[65]);
  EXPECT_EQ(0x00, ts->tx_buffer[66]);
  EXPECT_EQ(0x00, ts->tx_buffer[67]);
  EXPECT_EQ(0xA0, ts->tx_buffer[68]);
  EXPECT_EQ(0x7F, ts->tx_buffer[69]);
  EXPECT_EQ(0x17, ts->tx_buffer[70]);
  EXPECT_EQ(0x00, ts->tx_buffer[71]);
  EXPECT_EQ(0x6E, ts->tx_buffer[72]);
  EXPECT_EQ(0x00, ts->tx_buffer[73]);
  EXPECT_EQ(0x00, ts->tx_buffer[74]);
  EXPECT_EQ(0x48, ts->tx_buffer[75]);
  EXPECT_EQ(0x70, ts->tx_buffer[76]);
  EXPECT_EQ(0x30, ts->tx_buffer[77]);
  EXPECT_EQ(0x01, ts->tx_buffer[78]);
  EXPECT_EQ(0x00, ts->tx_buffer[79]);
  EXPECT_EQ(0x84, ts->tx_buffer[80]);
  EXPECT_EQ(0x05, ts->tx_buffer[81]);
  EXPECT_EQ(0x00, ts->tx_buffer[82]);
  EXPECT_EQ(0x4A, ts->tx_buffer[83]);
  EXPECT_EQ(0x01, ts->tx_buffer[84]);
  EXPECT_EQ(0x04, ts->tx_buffer[85]);
  EXPECT_EQ(0x20, ts->tx_buffer[86]);
  EXPECT_EQ(0x04, ts->tx_buffer[87]);
  EXPECT_EQ(0x24, ts->tx_buffer[88]);
  EXPECT_EQ(0x01, ts->tx_buffer[89]);
  EXPECT_EQ(0x2C, ts->tx_buffer[90]);
  EXPECT_EQ(0x71, ts->tx_buffer[91]);
  EXPECT_EQ(0x2C, ts->tx_buffer[92]);
  EXPECT_EQ(0x66, ts->tx_buffer[93]);

  // verify that connection added to list
  ASSERT_EQ(1, session.connections_.size());
  EXPECT_EQ(0, conn_num);
  EXPECT_EQ(0x0195, session.connections_[0].originator_vendor_id);
  EXPECT_EQ(0x00004321, session.connections_[0].originator_sn);
  EXPECT_EQ(0x6789, session.connections_[0].connection_sn);
  EXPECT_EQ(0x780C0002, session.connections_[0].o_to_t_connection_id);
  EXPECT_EQ(0x00020004, session.connections_[0].t_to_o_connection_id);
  EXPECT_EQ(6, session.connections_[0].timeout_tick_size);
  EXPECT_EQ(80, session.connections_[0].timeout_ticks);
  EXPECT_EQ(0x00177FA0, session.connections_[0].o_to_t_rpi);
  EXPECT_EQ(0x00013070, session.connections_[0].t_to_o_rpi);
  EXPECT_EQ(0x006E, session.connections_[0].o_to_t_buffer_size);
  EXPECT_EQ(0x0584, session.connections_[0].t_to_o_buffer_size);
  EXPECT_EQ(0x001781D0, session.connections_[0].o_to_t_api);
  EXPECT_EQ(0x00025CD8, session.connections_[0].t_to_o_api);
  // TODO: verify path ?

  unsigned char close_resp_packet[] = {
    0x6F, 0x00, 0x1E, 0x00, 0xEF, 0xBE, 0xAD, 0xDE,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xB2, 0x00, 0x0E, 0x00,
    0xCE, 0x00, 0x00, 0x00, 0x89, 0x67, 0x95, 0x01,
    0x21, 0x43, 0x00, 0x00, 0x00, 0x00,
  };

  ts->rx_buffer = buffer(close_resp_packet);
  ts->clearTxBuffer();
  session.closeConnection(conn_num);

  // check the forward close packet
  EXPECT_EQ(62, ts->tx_count);
  // command
  EXPECT_EQ(0x6F, ts->tx_buffer[0]);
  EXPECT_EQ(0x00, ts->tx_buffer[1]);
  // length
  EXPECT_EQ(0x26, ts->tx_buffer[2]);
  EXPECT_EQ(0x00, ts->tx_buffer[3]);
  // session handle
  EXPECT_EQ(0xEF, ts->tx_buffer[4]);
  EXPECT_EQ(0xBE, ts->tx_buffer[5]);
  EXPECT_EQ(0xAD, ts->tx_buffer[6]);
  EXPECT_EQ(0xDE, ts->tx_buffer[7]);
  // status
  EXPECT_EQ(0x00, ts->tx_buffer[8]);
  EXPECT_EQ(0x00, ts->tx_buffer[9]);
  EXPECT_EQ(0x00, ts->tx_buffer[10]);
  EXPECT_EQ(0x00, ts->tx_buffer[11]);
  // sender context
  EXPECT_EQ(0x00, ts->tx_buffer[12]);
  EXPECT_EQ(0x00, ts->tx_buffer[13]);
  EXPECT_EQ(0x00, ts->tx_buffer[14]);
  EXPECT_EQ(0x00, ts->tx_buffer[15]);
  EXPECT_EQ(0x00, ts->tx_buffer[16]);
  EXPECT_EQ(0x00, ts->tx_buffer[17]);
  EXPECT_EQ(0x00, ts->tx_buffer[18]);
  EXPECT_EQ(0x00, ts->tx_buffer[19]);
  // options
  EXPECT_EQ(0x00, ts->tx_buffer[20]);
  EXPECT_EQ(0x00, ts->tx_buffer[21]);
  EXPECT_EQ(0x00, ts->tx_buffer[22]);
  EXPECT_EQ(0x00, ts->tx_buffer[23]);
  // interface handle
  EXPECT_EQ(0x00, ts->tx_buffer[24]);
  EXPECT_EQ(0x00, ts->tx_buffer[25]);
  EXPECT_EQ(0x00, ts->tx_buffer[26]);
  EXPECT_EQ(0x00, ts->tx_buffer[27]);
  // timeout
  EXPECT_EQ(0x00, ts->tx_buffer[28]);
  EXPECT_EQ(0x00, ts->tx_buffer[29]);
  // items
  EXPECT_EQ(0x02, ts->tx_buffer[30]);
  EXPECT_EQ(0x00, ts->tx_buffer[31]);
  // null address item
  EXPECT_EQ(0x00, ts->tx_buffer[32]);
  EXPECT_EQ(0x00, ts->tx_buffer[33]);
  EXPECT_EQ(0x00, ts->tx_buffer[34]);
  EXPECT_EQ(0x00, ts->tx_buffer[35]);
  // UCMM data type
  EXPECT_EQ(0xB2, ts->tx_buffer[36]);
  EXPECT_EQ(0x00, ts->tx_buffer[37]);
  // length
  EXPECT_EQ(0x16, ts->tx_buffer[38]);
  EXPECT_EQ(0x00, ts->tx_buffer[39]);
  // service code
  EXPECT_EQ(0x4E, ts->tx_buffer[40]);
  // path length and path
  EXPECT_EQ(0x02, ts->tx_buffer[41]);
  EXPECT_EQ(0x20, ts->tx_buffer[42]);
  EXPECT_EQ(0x06, ts->tx_buffer[43]);
  EXPECT_EQ(0x24, ts->tx_buffer[44]);
  EXPECT_EQ(0x01, ts->tx_buffer[45]);
  // actual data
  EXPECT_EQ(0x06, ts->tx_buffer[46]);
  EXPECT_EQ(0x50, ts->tx_buffer[47]);
  EXPECT_EQ(0x89, ts->tx_buffer[48]);
  EXPECT_EQ(0x67, ts->tx_buffer[49]);
  EXPECT_EQ(0x95, ts->tx_buffer[50]);
  EXPECT_EQ(0x01, ts->tx_buffer[51]);
  EXPECT_EQ(0x21, ts->tx_buffer[52]);
  EXPECT_EQ(0x43, ts->tx_buffer[53]);
  EXPECT_EQ(0x00, ts->tx_buffer[54]);
  EXPECT_EQ(0x00, ts->tx_buffer[55]);
  EXPECT_EQ(0x02, ts->tx_buffer[56]);
  EXPECT_EQ(0x00, ts->tx_buffer[57]);
  EXPECT_EQ(0x20, ts->tx_buffer[58]);
  EXPECT_EQ(0x02, ts->tx_buffer[59]);
  EXPECT_EQ(0x24, ts->tx_buffer[60]);
  EXPECT_EQ(0x01, ts->tx_buffer[61]);

  ASSERT_EQ(0, session.connections_.size());
}

TEST_F(SessionTest, test_receive_io_packet)
{
  unsigned char io_packet[] = {
    0x02, 0x00, 0x02, 0x80, 0x08, 0x00, 0x04, 0x00,
    0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0xB1, 0x00,
    0x62, 0x00, 0xA1, 0x00, 0x76, 0x53, 0x04, 0x00,
    0x64, 0x96, 0x00, 0x00, 0x18, 0xBE, 0x97, 0x8A,
    0x19, 0xA7, 0x00, 0x00, 0x03, 0x00, 0x07, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x08, 0x07, 0x88, 0x33, 0xAE, 0x31,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
    0x00, 0x00, 0x14, 0x00, 0x52, 0x08, 0x42, 0x08,
    0x52, 0x08, 0x40, 0x08, 0x52, 0x08, 0x40, 0x08,
    0x53, 0x08, 0x58, 0x08, 0x52, 0x08, 0x40, 0x08,
    0x58, 0x08, 0x58, 0x08, 0x58, 0x08, 0x5E, 0x08,
    0x67, 0x08, 0x5D, 0x08, 0x67, 0x08, 0x5E, 0x08,
    0x5E, 0x08, 0x6F, 0x08,
  };

  ts_io->rx_buffer = buffer(io_packet);
  CPFPacket pkt = session.receiveIOPacket();
  ASSERT_EQ(2, pkt.getItemCount());
  EXPECT_EQ(0x8002, pkt.getItems()[0].getItemType());
  EXPECT_EQ(8, pkt.getItems()[0].getDataLength());
  EXPECT_EQ(0x00B1, pkt.getItems()[1].getItemType());
  EXPECT_EQ(0x0062, pkt.getItems()[1].getDataLength());

  SequencedAddressItem address;
  pkt.getItems()[0].getDataAs(address);
  EXPECT_EQ(0x00020004, address.connection_id);
  EXPECT_EQ(0x00000015, address.sequence_num);

  /*SequencedDataItem<MeasurementReport> data;
  pkt.getItems()[1].getDataAs(data);
  EXPECT_EQ(0x0062, data.getLength());
  EXPECT_EQ(0x00A1, data.sequence_num);
  EXPECT_EQ(0x00045376, data.header.scan_count);
  EXPECT_EQ(0x00009664, data.header.scan_rate);
  EXPECT_EQ(0x8a97BE18, data.header.scan_timestamp);
  EXPECT_EQ(0x0000A719, data.header.scan_beam_period);
  EXPECT_EQ(3, data.header.machine_state);
  EXPECT_EQ(7, data.header.machine_stop_reasons);
  EXPECT_EQ(0, data.header.active_zone_set);
  EXPECT_EQ(0, data.header.zone_inputs);
  EXPECT_EQ(0, data.header.detection_zone_status);
  EXPECT_EQ(1, data.header.output_status);
  EXPECT_EQ(0, data.header.input_status);
  EXPECT_EQ(0x0708, data.header.display_status);
  EXPECT_EQ(0x3388, data.header.non_safety_config_checksum);
  EXPECT_EQ(0x31AE, data.header.safety_config_checksum);
  EXPECT_EQ(1, data.header.range_report_format);
  EXPECT_EQ(2, data.header.refletivity_report_format);
  EXPECT_EQ(20, data.header.num_beams);
  ASSERT_EQ(20, data.measurement_data.size());
  EXPECT_EQ(0x0852, data.measurement_data[0]);
  EXPECT_EQ(0x0842, data.measurement_data[1]);
  EXPECT_EQ(0x0852, data.measurement_data[2]);
  EXPECT_EQ(0x0840, data.measurement_data[3]);
  EXPECT_EQ(0x0852, data.measurement_data[4]);
  EXPECT_EQ(0x0840, data.measurement_data[5]);
  EXPECT_EQ(0x0853, data.measurement_data[6]);
  EXPECT_EQ(0x0858, data.measurement_data[7]);
  EXPECT_EQ(0x0852, data.measurement_data[8]);
  EXPECT_EQ(0x0840, data.measurement_data[9]);
  EXPECT_EQ(0x0858, data.measurement_data[10]);
  EXPECT_EQ(0x0858, data.measurement_data[11]);
  EXPECT_EQ(0x0858, data.measurement_data[12]);
  EXPECT_EQ(0x085E, data.measurement_data[13]);
  EXPECT_EQ(0x0867, data.measurement_data[14]);
  EXPECT_EQ(0x085D, data.measurement_data[15]);
  EXPECT_EQ(0x0867, data.measurement_data[16]);
  EXPECT_EQ(0x085E, data.measurement_data[17]);
  EXPECT_EQ(0x085E, data.measurement_data[18]);
  EXPECT_EQ(0x086F, data.measurement_data[19]);*/
}

} // namespace eip
