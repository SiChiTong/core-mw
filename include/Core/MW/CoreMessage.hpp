/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/Message.hpp>
#include <Core/MW/CoreType.hpp>

/* Macros that allow us to keep the python code generator clean */
#define CORE_MESSAGE_BEGIN(__name__)  struct __name__: \
   public Core::MW::Message {
#define CORE_MESSAGE_FIELD(__name__, __type__, __size__) \
public: \
   Core::MW::CoreTypeTraits<Core::MW::CoreType::__type__, __size__>::Type __name__;
#define CORE_MESSAGE_END \
   } \
   CORE_PACKED;
