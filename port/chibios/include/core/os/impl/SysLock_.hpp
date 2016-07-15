/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/os/namespace.hpp>
#include <core/common.hpp>
#include <ch.h>

NAMESPACE_CORE_OS_BEGIN


class SysLock_:
   private core::Uncopyable
{
private:
   SysLock_();

public:
   static void
   acquire();

   static void
   release();
};


inline
void
SysLock_::acquire()
{
   chSysLock();
}

inline
void
SysLock_::release()
{
   chSysUnlock();
}

NAMESPACE_CORE_OS_END
