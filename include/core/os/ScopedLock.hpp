/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/os/namespace.hpp>
#include <core/common.hpp>

NAMESPACE_CORE_OS_BEGIN


template <typename Lockable>
class ScopedLock:
   private Uncopyable
{
private:
   Lockable& lock;

public:
   ScopedLock(
      Lockable& lock
   );
   ~ScopedLock();
};


template <typename Lockable>
inline
ScopedLock<Lockable>::ScopedLock(
   Lockable& lock
)
   :
   lock(lock)
{
   this->lock.acquire();
}

template <typename Lockable>
inline
ScopedLock<Lockable>::~ScopedLock()
{
   this->lock.release();
}

NAMESPACE_CORE_OS_END
