/**
Software License Agreement (BSD)

\file      buffer_writer_test.cpp
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
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>

#include "odva_ethernetip/serialization/buffer_writer.h"

using boost::shared_ptr;
using boost::make_shared;
using namespace boost::asio;
using namespace eip::serialization;

class BufferWriterTest : public :: testing :: Test
{

};

TEST_F(BufferWriterTest, test_write_int)
{
  unsigned char d[8];
  shared_ptr<Writer> writer = make_shared<BufferWriter> (buffer(d));
  EIP_UDINT value = 0x123455AA;
  writer->write(value);
  value = 0xDEADBEEF;
  writer->write(value);

  EXPECT_EQ(8, writer->getByteCount());
  EXPECT_EQ(0xAA, d[0]);
  EXPECT_EQ(0x55, d[1]);
  EXPECT_EQ(0x34, d[2]);
  EXPECT_EQ(0x12, d[3]);
  EXPECT_EQ(0xEF, d[4]);
  EXPECT_EQ(0xBE, d[5]);
  EXPECT_EQ(0xAD, d[6]);
  EXPECT_EQ(0xDE, d[7]);
}

TEST_F(BufferWriterTest, test_write_int_short_buffer)
{
  unsigned char d[7];
  shared_ptr<Writer> writer = make_shared<BufferWriter>(buffer(d));
  EIP_UDINT value = 0x123455AA;
  writer->write(value);
  value = 0xDEADBEEF;
  ASSERT_THROW(writer->write(value), std::length_error);
}

TEST_F(BufferWriterTest, test_write_bytes)
{
  unsigned char d[8];
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  shared_ptr<Writer> writer = make_shared<BufferWriter>(buffer(d));
  writer->writeBytes((char*)input, sizeof(input));
  EXPECT_EQ(8, writer->getByteCount());
  EXPECT_EQ(0xFF, d[0]);
  EXPECT_EQ(0xFE, d[1]);
  EXPECT_EQ(0xFD, d[2]);
  EXPECT_EQ(0xFC, d[3]);
  EXPECT_EQ(0xFB, d[4]);
  EXPECT_EQ(0xFA, d[5]);
  EXPECT_EQ(0x12, d[6]);
  EXPECT_EQ(0x34, d[7]);
}

TEST_F(BufferWriterTest, test_write_bytes_short_buffer)
{
  char d[7];
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  shared_ptr<Writer> writer = make_shared<BufferWriter>(buffer(d));
  ASSERT_THROW(writer->writeBytes(input, sizeof(input)), std::length_error);
}

TEST_F(BufferWriterTest, test_write_buffer)
{
  unsigned char d[8];
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  shared_ptr<Writer> writer = make_shared<BufferWriter>(buffer(d));
  writer->writeBuffer(buffer(input));
  EXPECT_EQ(8, writer->getByteCount());
  EXPECT_EQ(0xFF, d[0]);
  EXPECT_EQ(0xFE, d[1]);
  EXPECT_EQ(0xFD, d[2]);
  EXPECT_EQ(0xFC, d[3]);
  EXPECT_EQ(0xFB, d[4]);
  EXPECT_EQ(0xFA, d[5]);
  EXPECT_EQ(0x12, d[6]);
  EXPECT_EQ(0x34, d[7]);
}

TEST_F(BufferWriterTest, test_write_buffer_short_buffer)
{
  unsigned char d[7];
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  shared_ptr<Writer> writer = make_shared<BufferWriter>(buffer(d));
  ASSERT_THROW(writer->writeBuffer(buffer(input)), std::length_error);
}
