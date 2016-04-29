/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Mutex.hpp>

NAMESPACE_CORE_MW_BEGIN


class ReMutex:
   private Uncopyable
{
private:
   size_t counter;
   Mutex  mutex;

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
   SysLock::acquire();

   acquire_unsafe();
   SysLock::release();
}

inline
void
ReMutex::release()
{
   SysLock::acquire();

   release_unsafe();
   SysLock::release();
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
