/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/Message.hpp>
#include <core/mw/CoreType.hpp>

/* Macros that allow us to keep the python code generator clean */
#define CORE_MESSAGE_BEGIN(__name__)  struct __name__: \
    public core::mw::Message { \
    public: \
        using MessageType = __name__;
#define CORE_MESSAGE_FIELD(__name__, __type__, __size__) \
public: \
    core::mw::CoreTypeTraits<core::mw::CoreType::__type__, __size__>::Type __name__;
#define CORE_MESSAGE_SIGNATURE(__s__) \
public: \
    static const core::mw::Message::Signature SIGNATURE = __s__; \
    static constexpr core::mw::Message::Signature getMessageSignature() { \
        return __s__; \
    }
#define CORE_MESSAGE_END \
    } \
    CORE_PACKED_ALIGNED;

#define CORE_MESSAGE_FORMAT_BEGIN(__format__) \
public: \
    static int format(char* buffer, size_t length, const MessageType &m) { \
        return snprintf (buffer, length, __format__

#define CORE_MESSAGE_FORMAT_END \
    ); \
    }

#define CORE_MESSAGE_FORMAT_FIELD(__field__) , m.__field__
