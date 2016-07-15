/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/os/namespace.hpp>
#include <core/common.hpp>
#include <core/os/impl/Mutex_.hpp>

NAMESPACE_CORE_OS_BEGIN


class Mutex:
   private Uncopyable
{
private:
   Mutex_ impl;

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
   Mutex();
   explicit
   Mutex(
      bool initialize
   );
};


inline
void
Mutex::initialize()
{
   impl.initialize();
}

inline
void
Mutex::acquire_unsafe()
{
   impl.acquire_unsafe();
}

inline
void
Mutex::release_unsafe()
{
   impl.release_unsafe();
}

inline
void
Mutex::acquire()
{
   impl.acquire();
}

inline
void
Mutex::release()
{
   impl.release();
}

inline
Mutex::Mutex()
   :
   impl()
{}


inline
Mutex::Mutex(
   bool initialize
)
   :
   impl(initialize)
{}


NAMESPACE_CORE_OS_END
