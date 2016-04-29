/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Time.hpp>
#include <Core/MW/Thread.hpp>
#include <ch.h>

NAMESPACE_CORE_MW_BEGIN


class SpinEvent_
{
public:
   typedef ::eventmask_t Mask;

private:
   typedef ::thread_t ChThread;

private:
   Thread* threadp;

public:
   Thread*
   get_thread() const;

   void
   set_thread(
      Thread* threadp
   );

   void
   signal_unsafe(
      unsigned event_index
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
   SpinEvent_(
      Thread* threadp = & Thread::self()
   );
};


inline
Thread*
SpinEvent_::get_thread() const
{
   return threadp;
}

inline
void
SpinEvent_::set_thread(
   Thread* threadp
)
{
   this->threadp = threadp;
}

inline
void
SpinEvent_::signal_unsafe(
   unsigned event_index
)
{
   if (threadp != NULL) {
      CORE_ASSERT(event_index < 8 * sizeof(Mask));
      chEvtSignalI(reinterpret_cast<ChThread*>(threadp), 1 << event_index);
   }
}

inline
void
SpinEvent_::signal(
   unsigned event_index
)
{
   chSysLock();
   signal_unsafe(event_index);
   chSysUnlock();
}

inline
SpinEvent_::Mask
SpinEvent_::wait(
   const Time& timeout
)
{
   systime_t ticks;

   if (timeout == Time::IMMEDIATE) {
      ticks = TIME_IMMEDIATE;
   } else if (timeout == Time::INFINITE) {
      ticks = TIME_INFINITE;
   } else if (timeout.to_us_raw() > 100000) {
      ticks = MS2ST(timeout.to_ms_raw());
   } else {
      ticks = US2ST(timeout.to_us_raw());
   }

   return chEvtWaitAnyTimeout(ALL_EVENTS, ticks);
}

inline
SpinEvent_::SpinEvent_(
   Thread* threadp
)
   :
   threadp(threadp)
{}


NAMESPACE_CORE_MW_END
