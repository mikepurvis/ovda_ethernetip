/**
Software License Agreement (BSD)

\file      eip_types.h
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

#ifndef ODVA_ETHERNETIP_EIP_TYPES_H
#define ODVA_ETHERNETIP_EIP_TYPES_H

#include <stdint.h>

#define EIP_PROTOCOL_VERSION 1

// EtherNet/IP type defs
typedef bool      EIP_BOOL;   //< 1-bit value
typedef uint8_t   EIP_BYTE;   //< 8-bit string
typedef uint8_t   EIP_USINT;  //< unsigned 8-bit "short" integer
typedef int8_t    EIP_SINT;   //< signed 8-bit "short" integer
typedef uint16_t  EIP_WORD;   //< 16-bit string
typedef uint16_t  EIP_UINT;   //< unsigned 16-bit integer
typedef int16_t   EIP_INT;    //< signed 16-bit integer
typedef uint32_t  EIP_DWORD;  //< 32-bit string
typedef uint32_t  EIP_UDINT;  //< 32-bit unsigned integer
typedef int32_t   EIP_DINT;   //< 32-bit signed integer

typedef enum
{
  EIP_BOOL_CODE  = 0xC1,
  EIP_BYTE_CODE  = 0xD1,
  EIP_USINT_CODE = 0xC6,
  EIP_SINT_CODE  = 0xC2,
  EIP_WORD_CODE  = 0xD2,
  EIP_UINT_CODE  = 0xC7,
  EIP_INT_CODE   = 0xC3,
  EIP_DWORD_CODE = 0xD3,
  EIP_UDINT_CODE = 0xC8,
  EIP_DINT_CODE  = 0xC4,
} EIP_TYPE_CODE;

typedef enum
{
  EIP_CMD_NOP                 = 0x0000,
  EIP_CMD_LIST_SERVICES       = 0x0004,
  EIP_CMD_LIST_IDENTITY       = 0x0063,
  EIP_CMD_LIST_INTERFACES     = 0x0064,
  EIP_CMD_REGISTER_SESSION    = 0x0065,
  EIP_CMD_UNREGISTER_SESSION  = 0x0066,
  EIP_CMD_SEND_RR_DATA        = 0x006F,
  EIP_CMD_SEND_UNIT_DATA      = 0x0070,
  EIP_CMD_INDICATE_STATUS     = 0x0072,
  EIP_CMD_CANCEL              = 0x0073,
} EIP_CMD_T;

typedef enum
{
  EIP_STATUS_SUCCESS          = 0x0000,
  EIP_STATUS_INVALID_CMD      = 0x0001,
  EIP_STATUS_OUT_OF_MEMORY    = 0x0002,
  EIP_STATUS_INVALI_DATA      = 0x0003,
  EIP_STATUS_INVALID_SESSION  = 0x0064,
  EIP_STATUS_INVALID_LENGTH   = 0x0065,
  EIP_STATUS_INVALID_VERSION  = 0x0069,
} EIP_STATUS_T;

typedef enum
{
  EIP_ITEM_NULL                       = 0x0000,
  EIP_ITEM_LIST_IDENTITY_RESPONSE     = 0x000C,
  EIP_ITEM_CONNECTION_BASED           = 0x00A1,
  EIP_ITEM_CONNECTED_TRANSPORT_PACKET = 0x00B1,
  EIP_ITEM_UNCONNECTED_MESSAGE        = 0x00B2,
  EIP_ITEM_LIST_SERVICES_RESPONSE     = 0x0100,
  EIP_ITEM_SOCKADDR_INFO_ORIGINATOR   = 0x8000,
  EIP_ITEM_SOCKADDR_INFO_TARGET       = 0x8001,
  EIP_ITEM_SEQ_ADDRESS                = 0x8002,
} EIP_ITEM_T;

typedef enum
{
  CONN_PRIORITY_LOW       = 0,
  CONN_PRIORITY_HIGH      = 1,
  CONN_PRIORITY_SCHEDULED = 2,
  CONN_PRIORITY_URGENT    = 3,
} CONN_PRIORITY_T;

typedef enum
{
  CONN_TYPE_NULL      = 0,
  CONN_TYPE_MULTICAST = 1,
  CONN_TYPE_P2P       = 2,
  CONN_TYPE_RESERVED  = 3,
} CONN_TYPE_T;

typedef struct
{
  virtual void f() {}; // This allows dynamic_cast
  /// Assembly ID for this endpoint of the connection
  EIP_USINT assembly_id;
  /// Buffer size to be used for routing
  EIP_UINT buffer_size;
  /// Request packet interval
  EIP_UDINT rpi;
} EIP_CONNECTION_INFO_T;

typedef struct _EIP_CONNECTION_INFO_ADVANCED_T : EIP_CONNECTION_INFO_T
{
  // Variable Size: default o_to_t false, default t_to_o true
  bool variable_size;
  // Priority: default CONN_PRIORITY_SCHEDULED
  CONN_PRIORITY_T priority;
  //Connection Type: default CONN_TYPE_P2P
  CONN_TYPE_T type;
  // Connection Type: default false
  bool shared;
} EIP_CONNECTION_INFO_ADVANCED_T;

#endif  // ODVA_ETHERNETIP_EIP_TYPES_H
