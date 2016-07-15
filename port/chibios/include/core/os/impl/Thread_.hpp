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

class MemoryPool_;
class Time;


class Thread_
{
private:
   ::thread_t impl;

public:
   enum PriorityEnum {
      READY     = NOPRIO,
      IDLE      = IDLEPRIO,
      LOWEST    = LOWPRIO,
      NORMAL    = NORMALPRIO,
      HIGHEST   = HIGHPRIO - 1,
      INTERRUPT = HIGHPRIO
   };

   enum {
      OK = MSG_OK
   };

   typedef tprio_t   Priority;
   typedef ::tfunc_t Function;
   typedef msg_t     Return;
   typedef void*     Argument;

public:
   const char*
   get_name() const;

   void
   set_name(
      const char* namep
   );


   ::thread_t & get_impl();

private:
   Thread_();

public:
   static size_t
   compute_stack_size(
      size_t userlen
   );

   static Thread_*
   create_static(
      void*       stackp,
      size_t      stacklen,
      Priority    priority,
      Function    threadf,
      void*       argp,
      const char* namep = NULL
   );

   static Thread_*
   create_heap(
      void*       heapp,
      size_t      stacklen,
      Priority    priority,
      Function    threadf,
      void*       argp,
      const char* namep = NULL
   );

   static Thread_*
   create_pool(
      MemoryPool_& mempool,
      Priority     priority,
      Function     threadf,
      void*        argp,
      const char*  namep = NULL
   );

   static Thread_&
   self();

   static Priority
   get_priority();

   static void
   set_priority(
      Priority priority
   );

   static void
   yield();

   static void
   sleep(
      const Time& delay
   );

   static void
   sleep_until(
      const Time& time
   );

   static Return
   sleep();

   static void
   wake(
      Thread_& thread,
      Return   msg
   );

   static void
   exit(
      uint32_t msg
   );

   static bool
   join(
      Thread_& thread
   );

   static bool
   should_terminate();
};


NAMESPACE_CORE_OS_END

#include <core/os/impl/MemoryPool_.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_OS_BEGIN


inline
const char*
Thread_::get_name() const
{
   return chRegGetThreadNameX((thread_t*)&impl);
}

inline
void
Thread_::set_name(
   const char* namep
)
{
   return chRegSetThreadNameX((thread_t*)&impl, namep);
}

inline
  ::thread_t& Thread_::get_impl() {
   return impl;
}


inline
size_t
Thread_::compute_stack_size(
   size_t userlen
)
{
   return THD_WORKING_AREA_SIZE(userlen);
}

inline
Thread_*
Thread_::create_static(
   void*       stackp,
   size_t      stacklen,
   Priority    priority,
   Function    threadf,
   void*       argp,
   const char* namep
)
{
   Thread_* threadp = reinterpret_cast<Thread_*>(chThdCreateStatic(stackp, stacklen, static_cast<tprio_t>(priority), threadf, argp));

#if CH_USE_REGISTRY
   if (threadp != NULL) {
      chRegSetThreadNameX(threadp, namep);
   }

#else
   (void)namep;
#endif
   return threadp;
}

inline
Thread_*
Thread_::create_heap(
   void*       heapp,
   size_t      stacklen,
   Priority    priority,
   Function    threadf,
   void*       argp,
   const char* namep
)
{
   Thread_* threadp = reinterpret_cast<Thread_*>(chThdCreateFromHeap(reinterpret_cast<memory_heap_t*>(heapp), stacklen, priority, threadf, argp));

#if CH_USE_REGISTRY
   if (threadp != NULL) {
      chRegSetThreadNameX(threadp, namep);
   }

#else
   (void)namep;
#endif
   return threadp;
}

inline
Thread_*
Thread_::create_pool(
   MemoryPool_& mempool,
   Priority     priority,
   Function     threadf,
   void*        argp,
   const char*  namep
)
{
   Thread_* threadp = reinterpret_cast<Thread_*>(chThdCreateFromMemoryPool(&mempool.get_impl(), priority, threadf, argp));

#if CH_USE_REGISTRY
   if (threadp != NULL) {
      chRegSetThreadNameX(threadp, namep);
   }

#else
   (void)namep;
#endif
   return threadp;
}

inline
Thread_&
Thread_::self()
{
   return reinterpret_cast<Thread_&>(*chThdGetSelfX());
}

inline
Thread_::Priority
Thread_::get_priority()
{
   return chThdGetPriorityX();
}

inline
void
Thread_::set_priority(
   Priority priority
)
{
   chThdSetPriority(priority);
}

inline
void
Thread_::yield()
{
   chThdYield();
}

inline
void
Thread_::sleep(
   const Time& delay
)
{
   if (delay.raw > 0) {
      if (delay.raw < 1000000) {
         chThdSleepMicroseconds(delay.raw);
      } else {
         chThdSleepSeconds(delay.to_s_raw());
      }
   } else {
      chThdYield();
   }
}

inline
void
Thread_::sleep_until(
   const Time& time
)
{
   if (time.raw > 0) {
      chThdSleepUntil(US2ST(time.to_us_raw()));
   } else {
      chThdYield();
   }
}

inline
Thread_::Return
Thread_::sleep()
{
   chSchGoSleepS(CH_STATE_SUSPENDED);

   return chThdGetSelfX()->p_u.rdymsg;
}

inline void
Thread_::wake(
   Thread_&        thread,
   Thread_::Return msg
)
{
   thread.impl.p_u.rdymsg = msg;
   chSchReadyI(&thread.impl);
}

inline
void
Thread_::exit(
   uint32_t msg
)
{
   chThdExitS(msg);
}

inline
bool
Thread_::join(
   Thread_& thread
)
{
   return chThdWait(&thread.impl) == MSG_OK;
}

inline
bool
Thread_::should_terminate()
{
   return chThdShouldTerminateX();
}

NAMESPACE_CORE_OS_END
