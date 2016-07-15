/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Message.hpp>
#include <cstring>

NAMESPACE_CORE_MW_BEGIN


void
Message::acquire()
{
   core::os::SysLock::acquire();

   acquire_unsafe();
   core::os::SysLock::release();
}

bool
Message::release()
{
   core::os::SysLock::acquire();
   bool floating = release_unsafe();
   core::os::SysLock::release();

   return floating;
}

void
Message::reset()
{
   core::os::SysLock::acquire();

   reset_unsafe();
   core::os::SysLock::release();
}

void
Message::copy(
   Message&       to,
   const Message& from,
   size_t         type_size
)
{
   CORE_ASSERT(type_size >= sizeof(RefcountType));

   memcpy( // TODO: just copy the payload with payload_size instead of type_size
      &to.refcount + 1, &from.refcount + 1,
#if CORE_USE_BRIDGE_MODE
      type_size - (sizeof(Transport*) + sizeof(RefcountType))
#else
      type_size - sizeof(RefcountType)
#endif
   );
}

NAMESPACE_CORE_MW_END
