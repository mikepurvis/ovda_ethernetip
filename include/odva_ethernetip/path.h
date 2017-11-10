/**
Software License Agreement (BSD)

\file      path.h
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

#ifndef ODVA_ETHERNETIP_PATH_H
#define ODVA_ETHERNETIP_PATH_H

#include <vector>

#include "odva_ethernetip/eip_types.h"
#include "odva_ethernetip/serialization/reader.h"
#include "odva_ethernetip/serialization/writer.h"
#include "odva_ethernetip/serialization/serializable.h"

using std::vector;
using std::size_t;

namespace eip {

using serialization::Serializable;
using serialization::Reader;
using serialization::Writer;

/**
 * Class to handle serialization of the Path data type in EthernetIP.
 * Note that this is not a complete implementation of all path segment types,
 * just the elements needed for the OS32c.
 */
class Path
{
public:

  /**
   * Construct an empty path.
   * @param pad_after_length set to true to enable pad byte after length when
   *   serializing the path.
   */
  Path(bool pad_after_length = false);

  /**
   * Shortcut to construct a path to the given logical class instance and
   * attribute IDs.
   * @param class_id Logical Class ID for the first segment of the path
   * @param instance_id Logical instance ID for the second segment of the path
   * @param attribute_id Logical instance ID for the third segment of the path
   * @param pad_after_length set to true to enable pad byte after length when
   *   serializing the path.
   */
  Path(EIP_USINT class_id, EIP_USINT instance_id, EIP_USINT attribute_id,
    bool pad_after_length = false);
  Path(EIP_USINT class_id, EIP_USINT instance_id, EIP_UINT attribute_id,
    bool pad_after_length = false);

  /**
   * Shortcut to construct a path to the given logical class instance
   * @param class_id Logical Class ID for the first segment of the path
   * @param instance_id Logical instance ID for the second segment of the path
   * @param pad_after_length set to true to enable pad byte after length when
   *   serializing the path.
   */
  Path(EIP_USINT class_id, EIP_USINT instance_id);

  /**
   * In some special cases, need to add a pad byte after the length when
   * serializing the path. Set this flag to true to enable that behaviour.
   * @param pad if set to true, will add a pad byte after the path length when
   *   serializing the path.
   */
  void setPadAfterLength(bool pad)
  {
    pad_after_length_ = pad;
  }

  /**
   * Add a logical class segment
   * @param class_id ID Number of class to add to path
   */
  void addLogicalClass(EIP_USINT class_id);

  /**
   * Add a logical instance segment
   * @param instance_id ID Number of instance to add to path
   */
  void addLogicalInstance(EIP_USINT instance_id);

  /**
   * Add a logical attribute segment
   * @param attribute_id ID Number of attribute to add to path
   */
  void addLogicalAttribute(EIP_USINT attribute_id);
  void addLogicalAttribute(EIP_UINT attribute_id);

  /**
   * Add a logical connection point segment
   * @param ID number of the connection point (usually assembly ID)
   */
  void addLogicalConnectionPoint(EIP_USINT connection_id);

  /**
   * Clear out anything currently in the path
   */
  void reset()
  {
    path_buf_.clear();
  }

  /**
   * Get the length of serialized data that would be produced if serialized
   * @return Total length in bytes to be serialized
   */
  virtual size_t getLength() const;

  /**
   * Serialize data into the given buffer
   * @param writer Writer to use for serialization
   * @return the writer again
   * @throw std::length_error if the buffer is too small for the header data
   */
  virtual Writer& serialize(Writer& writer) const
  {
    serialize(writer, pad_after_length_);
    return writer;
  }

  /**
   * Serialize data into the given buffer with a flag for adding a pad after
   * the length field.
   * @param writer Writer to use for serialization
   * @param pad_after_length set to true to add a byte after the length field
   * @return the writer again
   * @throw std::length_error if the buffer is too small for the header data
   */
  virtual Writer& serialize(Writer& writer, bool pad_after_length) const;

  /**
   * Not actually implemented
   */
  virtual Reader& deserialize(Reader& reader, size_t length)
  {
    throw std::logic_error("Not implemented");
  }

  /**
   * Not actually implemented
   */
  virtual Reader& deserialize(Reader& reader)
  {
    throw std::logic_error("Not implemented");
  }

  /**
   * Prints path bytes at std output
   */
  void print() const;

private:
  bool pad_after_length_;
  vector<EIP_USINT> path_buf_;

  /**
   * Helper to put a segment into the current path buffer
   * @param type Type of segment to add
   * @param data Data to add to path
   */
  void addSegment(EIP_USINT type, EIP_USINT data);
  void addSegment(EIP_USINT type, EIP_UINT data);
};

} // namespace eip

#endif  // ODVA_ETHERNETIP_PATH_H
