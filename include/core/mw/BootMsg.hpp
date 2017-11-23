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
#include <core/String.hpp>

NAMESPACE_CORE_MW_BEGIN

namespace bootloader {
using ProductUID = uint16_t;
using ModuleUID  = uint32_t;
using ModuleType = core::String<12>;
using ModuleName = core::String<16>;

static const uint32_t LONG_MESSAGE_LENGTH  = 48;
static const uint32_t SHORT_MESSAGE_LENGTH = 8;

static const uint32_t MAXIMUM_MESSAGE_LENGTH = 48;

enum class MessageType : uint8_t {
    NONE           = 0x00,
    REQUEST        = 0x01,
    IDENTIFY_SLAVE = 0x02,
    SELECT_SLAVE   = 0x10,
    DESELECT_SLAVE = 0x11,

    ERASE_CONFIGURATION      = 0x04,
    ERASE_PROGRAM            = 0x05,
    WRITE_PROGRAM_CRC        = 0x06,
    ERASE_USER_CONFIGURATION = 0x07,

    // MODULE_NAME         = 0x25,
    // READ_MODULE_NAME    = 0x26,
    WRITE_MODULE_NAME   = 0x27,
    WRITE_MODULE_CAN_ID = 0x28,
    DESCRIBE_V1         = 0x29,
    DESCRIBE_V2         = 0x26,
    DESCRIBE_V3         = 0x30,

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
    MessageType command;
    uint8_t     sequenceId;

    const Message*
    asMessage()
    {
        return this;
    }
}

CORE_PACKED_ALIGNED;


struct EMPTY_PAYLOAD {};

template <std::size_t _MESSAGE_LENGTH>
class MessageBase:
    public Message
{
public:
    using ParentType = Message;
    static const std::size_t MESSAGE_LENGTH = _MESSAGE_LENGTH;

    uint8_t data[MESSAGE_LENGTH - 2];
}

CORE_PACKED_ALIGNED;


using BootMasterMsg = MessageBase<SHORT_MESSAGE_LENGTH>;
using BootMsg       = MessageBase<LONG_MESSAGE_LENGTH>;

template <typename _CONTAINER, MessageType _TYPE, typename _PAYLOAD>
class Message_:
    public _CONTAINER::Parent
{
public:
    using ContainerType = _CONTAINER;
    using PayloadType   = _PAYLOAD;

    Message_() :
        ContainerType(_TYPE)
    {}

    PayloadType data;

    uint8_t padding[ContainerType::MESSAGE_LENGTH - sizeof(ContainerType) - sizeof(data)];
}

CORE_PACKED_ALIGNED;


struct AcknowledgeMessage:
    public core::mw::Message {
    MessageType       command;
    uint8_t           sequenceId;
    AcknowledgeStatus status;
    MessageType       type;

    const Message*
    asMessage()
    {
        return this;
    }
}

CORE_PACKED_ALIGNED;

template <std::size_t _MESSAGE_LENGTH>
class AcknowledgeMessageBase:
    public AcknowledgeMessage
{
public:
    using ParentType = AcknowledgeMessage;
    static const std::size_t MESSAGE_LENGTH = _MESSAGE_LENGTH;

    uint8_t data[MESSAGE_LENGTH - sizeof(AcknowledgeMessage)];
}

CORE_PACKED_ALIGNED;

template <typename CONTAINER, typename PAYLOAD>
class AcknowledgeMessage_:
    public CONTAINER::ParentType
{
        // static_assert(sizeof(PAYLOAD) <= CONTAINER::MESSAGE_LENGTH - sizeof(AcknowledgeMessage), "Payload too large");
public:
    using ContainerType = CONTAINER;
    using PayloadType   = PAYLOAD;

    PAYLOAD data;

    uint8_t padding[ContainerType::MESSAGE_LENGTH - sizeof(AcknowledgeMessage) - sizeof(data)];
}

CORE_PACKED_ALIGNED;

namespace payload {
struct EMPTY {};

struct NAME {
    ModuleName name;
};

struct UID {
    ModuleUID uid;
};

struct UIDAndMaster {
    ModuleUID uid;
    uint8_t   masterID;
};

struct UIDAndCRC {
    ModuleUID uid;
    uint32_t  crc;
};

struct UIDAndID {
    ModuleUID uid;
    uint8_t   id;
};

struct UIDAndName {
    ModuleUID  uid;
    ModuleName name;
};

struct UIDAndAddress {
    ModuleUID uid;
    uint32_t  address;
};

struct IHex {
    enum Type : uint8_t {
        BEGIN = 0x01,
        DATA  = 0x02,
        END   = 0x03
    };

    using Data = char[44];

    Type type;
    Data string;
};

struct Announce {
    ModuleUID uid;
    uint8_t   version;
};

struct DescribeV1 {
    uint32_t   programFlashSize;
    uint16_t   userFlashSize;
    uint8_t    moduleId;
    ModuleType moduleType;
    ModuleName moduleName;
};

struct DescribeV2 {
    uint32_t   programFlashSize;
    uint32_t   confCRC;
    uint32_t   flashCRC;
    uint16_t   userFlashSize;
    uint8_t    moduleId;
    ModuleType moduleType;
    ModuleName moduleName;
};

struct DescribeV3 {
    uint32_t   programFlashSize;
    uint16_t   userFlashSize;
    uint16_t   tagsFlashSize;
    uint8_t    programValid;
    uint8_t    userValid;
    uint8_t    moduleId;
    ModuleType moduleType;
    ModuleName moduleName;
};
}

namespace messages {
// SLAVE -> MASTER
using Announce = Message_<BootMasterMsg, MessageType::REQUEST, payload::Announce>;

// MASTER -> SLAVE
using Bootload       = Message_<BootMsg, MessageType::BOOTLOAD, payload::EMPTY>;
using BootloadByName = Message_<BootMsg, MessageType::BOOTLOAD_BY_NAME, payload::NAME>;

using IdentifySlave = Message_<BootMsg, MessageType::IDENTIFY_SLAVE, payload::UID>;
using SelectSlave   = Message_<BootMsg, MessageType::SELECT_SLAVE, payload::UIDAndMaster>;
using DeselectSlave = Message_<BootMsg, MessageType::DESELECT_SLAVE, payload::UID>;

using EraseConfiguration = Message_<BootMsg, MessageType::ERASE_CONFIGURATION, payload::UID>;
using EraseProgram       = Message_<BootMsg, MessageType::ERASE_PROGRAM, payload::UID>;
using WriteProgramCrc    = Message_<BootMsg, MessageType::WRITE_PROGRAM_CRC, payload::UIDAndCRC>;

using DescribeV1 = Message_<BootMsg, MessageType::DESCRIBE_V1, payload::UID>;
using DescribeV2 = Message_<BootMsg, MessageType::DESCRIBE_V2, payload::UID>;
using DescribeV3 = Message_<BootMsg, MessageType::DESCRIBE_V3, payload::UID>;

using IHexData = Message_<BootMsg, MessageType::IHEX_READ, payload::IHex>;

using IHexRead = Message_<BootMsg, MessageType::IHEX_WRITE, payload::UIDAndAddress>;

using Reset = Message_<BootMsg, MessageType::RESET, payload::UID>;

//using ReadName = Message_<BootMsg, MessageType::READ_MODULE_NAME, payload::UID>;
using WriteModuleName = Message_<BootMsg, MessageType::WRITE_MODULE_NAME, payload::UIDAndName>;
using WriteModuleID   = Message_<BootMsg, MessageType::WRITE_MODULE_CAN_ID, payload::UIDAndID>;
}

using AcknowledgeMsg      = AcknowledgeMessageBase<LONG_MESSAGE_LENGTH>;
using AcknowledgeUID      = AcknowledgeMessage_<AcknowledgeMsg, payload::UID>;
using AcknowledgeDescribeV1 = AcknowledgeMessage_<AcknowledgeMsg, payload::DescribeV1>;
using AcknowledgeDescribeV2 = AcknowledgeMessage_<AcknowledgeMsg, payload::DescribeV2>;
using AcknowledgeDescribeV3 = AcknowledgeMessage_<AcknowledgeMsg, payload::DescribeV3>;
using AcknowledgeString   = AcknowledgeMessage_<AcknowledgeMsg, char[44]>;
}
NAMESPACE_CORE_MW_END
