/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/Message.hpp>
#include <core/mw/NamingTraits.hpp>
#include <core/Array.hpp>

NAMESPACE_CORE_MW_BEGIN

namespace bootloader {
using ProductUID = uint16_t;
using ModuleUID  = uint32_t;
using ModuleType = core::Array<char, 12>;
using ModuleName = core::Array<char, 14>;

static const uint32_t SHORT_MESSAGE_LENGTH = 8;
static const uint32_t LONG_MESSAGE_LENGTH  = 48;

static const uint32_t MAXIMUM_MESSAGE_LENGTH = 48;

enum class MessageType : uint8_t {
    NONE           = 0x00,
    REQUEST        = 0x01, //
    IDENTIFY_SLAVE = 0x02, //
    SELECT_SLAVE   = 0x10, //
    DESELECT_SLAVE = 0x11, //

    ERASE_CONFIGURATION      = 0x04,
    ERASE_PROGRAM            = 0x05,
    WRITE_PROGRAM_CRC        = 0x06,
    ERASE_USER_CONFIGURATION = 0x07,

    MODULE_NAME       = 0x25,
    READ_MODULE_NAME  = 0x26,
    WRITE_MODULE_NAME = 0x27,
    WRITE_MODULE_ID   = 0x28,
    DESCRIBE          = 0x29, //

    IHEX_WRITE = 0x50,
    IHEX_READ  = 0x51,

    RESET            = 0x60,
    BOOTLOAD         = 0x70,
    BOOTLOAD_BY_NAME = 0x71,

    MASTER_ADVERTISE = 0xA0,
    MASTER_IGNORE    = 0xA1,
    MASTER_FORCE     = 0xA2,

    ACK = 0xFF
};

enum class AcknowledgeStatus : uint8_t {
    NONE            = 0x00,
    OK              = 0x01,
    WRONG_UID       = 0x02,
    WRONG_SEQUENCE  = 0x03,
    DISCARD         = 0x04,
    NOT_SELECTED    = 0x05,
    NOT_IMPLEMENTED = 0x06,
    BROKEN          = 0x07,
    ERROR           = 0x08,
    IHEX_OK         = 0x09,
    DO_NOT_ACK      = 0x0A,
};

struct Message:
    public core::mw::Message {
    Message() : command(MessageType::NONE) {}

    Message(
        MessageType c
    ) :
        command(c)
    {}

    MessageType command;

    const Message*
    asMessage()
    {
        return this;
    }
}

CORE_PACKED_ALIGNED;


template <std::size_t _MESSAGE_LENGTH>
struct MessageBase:
    public Message {
    static const std::size_t MESSAGE_LENGTH = _MESSAGE_LENGTH;

    MessageBase() : Message(MessageType::NONE), sequenceId(0) {}

    MessageBase(
        MessageType c
    ) : Message(c), sequenceId(0)
    {}

    //MessageType command;
    uint8_t sequenceId;
    uint8_t data[MESSAGE_LENGTH - 2];

    void
    copyTo(
        uint8_t* buffer
    ) const
    {
        const uint8_t* f = reinterpret_cast<const uint8_t*>(this);
        uint8_t*       t = buffer;

        for (std::size_t i = 0; i < _MESSAGE_LENGTH; i++) {
            *(t++) = *(f++);
        }
    }

    void
    copyFrom(
        const uint8_t* buffer
    )
    {
        const uint8_t* f = buffer;
        uint8_t*       t = reinterpret_cast<uint8_t*>(this);

        for (std::size_t i = 0; i < _MESSAGE_LENGTH; i++) {
            *(t++) = *(f++);
        }
    }
}

CORE_PACKED_ALIGNED;
#if 0
template <typename _CONTAINER, MessageType _TYPE, typename _PAYLOAD>
struct Message_:
    public _CONTAINER {
    //static const std::size_t MESSAGE_LENGTH = _CONTAINER::MESSAGE_LENGTH;
    using ContainerType = _CONTAINER;
    using PayloadType   = _PAYLOAD;

    Message_() :
        ContainerType(_TYPE)
    {}

    PayloadType data;

    uint8_t padding[ContainerType::MESSAGE_LENGTH - sizeof(ContainerType) - sizeof(data)];
}

CORE_PACKED_ALIGNED;
#endif // if 0

using BootMasterMsg = MessageBase<SHORT_MESSAGE_LENGTH>;
using BootMsg       = MessageBase<LONG_MESSAGE_LENGTH>;
}


NAMESPACE_CORE_MW_END
