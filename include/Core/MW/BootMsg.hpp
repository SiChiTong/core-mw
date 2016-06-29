/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Message.hpp>
#include <Core/MW/NamingTraits.hpp>

NAMESPACE_CORE_MW_BEGIN
static const uint32_t MESSAGE_LENGTH = 48;

class BootMsg:
   public Message
{
public:
   enum MessageType : uint8_t {
      REQUEST        = 0x01, //
      IDENTIFY_SLAVE = 0x02, //
      SELECT_SLAVE   = 0x10, //
      DESELECT_SLAVE = 0x11, //

      ERASE_CONFIGURATION = 0x04,
      ERASE_PROGRAM       = 0x05,

      MODULE_NAME = 0x25,
      READ_NAME   = 0x26,
      WRITE_NAME  = 0x27,

      READ_FLASH_ABSOLUTE  = 0x80,
      WRITE_FLASH_ABSOLUTE = 0x81,

      IHEX = 0x50,

      RESET = 0x60,

      ACK = 0xFF
   };

   enum AcknowledgeStatus : uint8_t {
      OK = 0x01, //
      WRONG_UID,
      WRONG_SEQUENCE,
      DISCARD,
      NOT_SELECTED,
      NOT_IMPLEMENTED,
      BROKEN,
      ERROR
   };

public:
   MessageType command;
   uint8_t     seqId;
   uint8_t     payload[MESSAGE_LENGTH - sizeof(MessageType) - 1];
}

CORE_PACKED;

NAMESPACE_CORE_MW_END
