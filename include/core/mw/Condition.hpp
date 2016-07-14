/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/common.hpp>
#include <core/mw/impl/Condition_.hpp>

NAMESPACE_CORE_MW_BEGIN


class Condition:
   private Uncopyable
{
private:
   Condition_ impl;

public:
   void
   initialize();

   void
   signal_unsafe();

   void
   broadcast_unsafe();

   void
   wait_unsafe();

   bool
   wait_unsafe(
      const Time& timeout
   );

   void
   signal();

   void
   broadcast();

   void
   wait();

   bool
   wait(
      const Time& timeout
   );


public:
   Condition();
   explicit
   Condition(
      bool initialize
   );
};


inline
void
Condition::initialize()
{
   impl.initialize();
}

inline
void
Condition::signal_unsafe()
{
   impl.signal_unsafe();
}

inline
void
Condition::broadcast_unsafe()
{
   impl.broadcast_unsafe();
}

inline
void
Condition::wait_unsafe()
{
   impl.wait_unsafe();
}

inline
bool
Condition::wait_unsafe(
   const Time& timeout
)
{
   return impl.wait_unsafe(timeout);
}

inline
void
Condition::signal()
{
   impl.signal();
}

inline
void
Condition::broadcast()
{
   impl.broadcast();
}

inline
void
Condition::wait()
{
   impl.wait();
}

inline
bool
Condition::wait(
   const Time& timeout
)
{
   return impl.wait(timeout);
}

inline
Condition::Condition()
   :
   impl()
{}


inline
Condition::Condition(
   bool initialize
)
   :
   impl(initialize)
{}


NAMESPACE_CORE_MW_END
