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

NAMESPACE_CORE_MW_BEGIN

class Transport;


class MgmtMsg:
    public Message
{
public:
    enum TypeEnum {
//		RAW = 0x00,

        // Module messages
        ALIVE    = 0x11,
        STOP     = 0x12,
        REBOOT   = 0x13,
        BOOTLOAD = 0x14,

        // PubSub messages
        ADVERTISE          = 0x21,
        SUBSCRIBE_REQUEST  = 0x22,
        SUBSCRIBE_RESPONSE = 0x23,

        // Path messages
        PATH = 0x31,

        // Bootloader
        BOOTLOADER = 0x41,
    };

    enum {
        MAX_PAYLOAD_LENGTH = 31
    };

    struct PubSub {
        enum {
            MAX_RAW_PARAMS_LENGTH = 10
        };

        char     topic[NamingTraits < Topic > ::MAX_LENGTH];
        uint16_t payload_size;
        uint16_t queue_length;
        uint8_t  raw_params[MAX_RAW_PARAMS_LENGTH];
    }

    CORE_PACKED;

    struct Module {
        char    name[NamingTraits < Middleware > ::MAX_LENGTH];
        uint8_t reserved_;
        struct {
            unsigned stopped   : 1;
            unsigned rebooted  : 1;
            unsigned boot_mode : 1;
            unsigned reserved_ : 5;
        }

        CORE_PACKED flags;
    }

    CORE_PACKED;

public:
    union {
        uint8_t payload[MAX_PAYLOAD_LENGTH];
        PubSub  pubsub;
        Module  module;
    }

    CORE_PACKED;

    uint8_t type;
}

CORE_PACKED;


NAMESPACE_CORE_MW_END
