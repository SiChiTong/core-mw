/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>

#include <core/mw/NamingTraits.hpp>
#include <core/mw/Message.hpp>
#include <core/String.hpp>

#include <type_traits>

NAMESPACE_CORE_MW_BEGIN
namespace rpc {
static const uint32_t RPC_MESSAGE_LENGTH = 64;
static const uint32_t RPC_NAME_LENGTH    = 16;

using ModuleName = String<core::mw::NamingTraits<Middleware>::MAX_LENGTH>;
using RPCName    = String<RPC_NAME_LENGTH>;

enum class MessageType : uint8_t {
    NONE              = 0xFF,
    DISCOVER_REQUEST  = 0x10,
    DISCOVER_RESPONSE = 0x11,
    REQUEST           = 0x20,
    RESPONSE          = 0x21
};

class RPCMessage:
    public Message
{
public:
    struct Header {
        MessageType type;
        uint8_t     client_session;
        uint8_t     server_session;
        uint8_t     sequence;
        ModuleName  target_module_name;
        uint8_t     _rfu_padding;
    }

    CORE_PACKED;

    struct DiscoveryRequest {
        ModuleName client_name;
        RPCName    rpc_name;
    }

    CORE_PACKED;

    struct DiscoveryResponse {
        ModuleName server_name;
        RPCName    rpc_name;
    }

    CORE_PACKED;


    static const std::size_t PAYLOAD_SIZE = RPC_MESSAGE_LENGTH - sizeof(Header);

    Header header;

    union {
        uint8_t           payload[PAYLOAD_SIZE];
        DiscoveryRequest  discovery_request;
        DiscoveryResponse discovery_response;
    }

    CORE_PACKED;
}

CORE_PACKED_ALIGNED;
}
NAMESPACE_CORE_MW_END
