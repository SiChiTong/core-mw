/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/common.hpp>
#include <core/mw/impl/SpinEvent_.hpp>
#include <core/mw/Time.hpp>
#include <core/mw/Thread.hpp>

NAMESPACE_CORE_MW_BEGIN


class SpinEvent:
   private Uncopyable
{
public:
   typedef SpinEvent_::Mask Mask;

   enum {
      MAX_INDEX = (sizeof(Mask) * 8) - 1
   };

private:
   SpinEvent_ impl;

public:
   Thread*
   get_thread() const;

   void
   set_thread(
      Thread* threadp
   );

   void
   signal_unsafe(
      unsigned event_index,
      bool     mustReschedule = false
   );

   void
   signal(
      unsigned event_index
   );

   Mask
   wait(
      const Time& timeout
   );


public:
   SpinEvent(
      Thread* threadp = & Thread::self()
   );
};


inline
Thread*
SpinEvent::get_thread() const
{
   return impl.get_thread();
}

inline
void
SpinEvent::set_thread(
   Thread* threadp
)
{
   impl.set_thread(threadp);
}

inline
void
SpinEvent::signal_unsafe(
   unsigned event_index,
   bool     mustReschedule
)
{
   impl.signal_unsafe(event_index, mustReschedule);
}

inline
void
SpinEvent::signal(
   unsigned event_index
)
{
   impl.signal(event_index);
}

inline
SpinEvent::Mask
SpinEvent::wait(
   const Time& timeout
)
{
   return impl.wait(timeout);
}

inline
SpinEvent::SpinEvent(
   Thread* threadp
)
   :
   impl(threadp)
{}


NAMESPACE_CORE_MW_END
