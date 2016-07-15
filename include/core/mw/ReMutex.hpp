/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/os/Mutex.hpp>

NAMESPACE_CORE_MW_BEGIN


class ReMutex:
   private ::core::Uncopyable
{
private:
   size_t counter;
   ::core::os::Mutex  mutex;

public:
   void
   initialize();

   void
   acquire_unsafe();

   void
   release_unsafe();

   void
   acquire();

   void
   release();


public:
   ReMutex();
   explicit
   ReMutex(
      bool initialize
   );
};


inline
void
ReMutex::initialize()
{
   counter = 0;
   mutex.initialize();
}

inline
void
ReMutex::acquire_unsafe()
{
   if (++counter == 1) {
      mutex.acquire_unsafe();
   }
}

inline
void
ReMutex::release_unsafe()
{
   CORE_ASSERT(counter > 0);

   if (--counter == 0) {
      mutex.release_unsafe();
   }
}

inline
void
ReMutex::acquire()
{
   ::core::os::SysLock::acquire();
   acquire_unsafe();
   ::core::os::SysLock::release();
}

inline
void
ReMutex::release()
{
   ::core::os::SysLock::acquire();
   release_unsafe();
   ::core::os::SysLock::release();
}

inline
ReMutex::ReMutex()
   :
   counter(0),
   mutex()
{}


inline
ReMutex::ReMutex(
   bool initialize
)
   :
   counter(0),
   mutex(initialize)
{}


NAMESPACE_CORE_MW_END
