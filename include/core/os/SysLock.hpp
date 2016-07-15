/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/os/namespace.hpp>
#include <core/common.hpp>
#include <core/os/impl/SysLock_.hpp>

NAMESPACE_CORE_OS_BEGIN


class SysLock:
   private Uncopyable
{
public:
   class Scope:
      private Uncopyable
   {
public:
      Scope()
      {
         SysLock::acquire();
      }

      ~Scope()
      {
         SysLock::release();
      }
   };

private:
   SysLock();

public:
   static void
   acquire();

   static void
   release();
};


inline
void
SysLock::acquire()
{
   SysLock_::acquire();
}

inline
void
SysLock::release()
{
   SysLock_::release();
}

NAMESPACE_CORE_OS_END
