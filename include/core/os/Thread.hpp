/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/os/namespace.hpp>
#include <core/common.hpp>
#include <core/os/impl/Thread_.hpp>
#include <core/os/MemoryPool.hpp>

NAMESPACE_CORE_OS_BEGIN

class Time;

class Thread:
   private Uncopyable
{
public:
   using Priority = Thread_::Priority;
   using Function = Thread_::Function;
   using Return   = Thread_::Return;
   using Argument = Thread_::Argument;

   enum PriorityEnum : Priority {
      READY     = Thread_::READY,
      IDLE      = Thread_::IDLE,
      LOWEST    = Thread_::LOWEST,
      NORMAL    = Thread_::NORMAL,
      HIGHEST   = Thread_::HIGHEST,
      INTERRUPT = Thread_::INTERRUPT
   };

   enum {
      OK = Thread_::OK
   };

private:
   Thread_ impl;

public:
   const char*
   get_name() const;

   void
   set_name(
      const char* namep
   );


private:
   Thread();

public:
   static size_t
   compute_stack_size(
      size_t userlen
   );

   static Thread*
   create_static(
      void*       stackp,
      size_t      stacklen,
      Priority    priority,
      Function    threadf,
      void*       argp,
      const char* namep = NULL
   );

   static Thread*
   create_heap(
      void*       heapp,
      size_t      stacklen,
      Priority    priority,
      Function    threadf,
      void*       argp,
      const char* namep = NULL
   );


   template <typename T>
   static Thread*
   create_pool(
      MemoryPool<T>& mempool,
      Priority       priority,
      Function       threadf,
      void*          argp,
      const char*    namep = NULL
   );

   static Thread&
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
      Thread& thread,
      Return  msg
   );

   static void
   exit(
      uint32_t msg
   );

   static bool
   join(
      Thread& thread
   );

   static bool
   should_terminate();
};


inline
const char*
Thread::get_name() const
{
   return impl.get_name();
}

inline
void
Thread::set_name(
   const char* namep
)
{
   impl.set_name(namep);
}

inline
size_t
Thread::compute_stack_size(
   size_t userlen
)
{
   return Thread_::compute_stack_size(userlen);
}

inline
Thread*
Thread::create_static(
   void*       stackp,
   size_t      stacklen,
   Priority    priority,
   Function    threadf,
   void*       argp,
   const char* namep
)
{
   return reinterpret_cast<Thread*>(
      Thread_::create_static(stackp, stacklen, priority, threadf, argp, namep)
   );
}

inline
Thread*
Thread::create_heap(
   void*       heapp,
   size_t      stacklen,
   Priority    priority,
   Function    threadf,
   void*       argp,
   const char* namep
)
{
   return reinterpret_cast<Thread*>(
      Thread_::create_heap(heapp, stacklen, priority, threadf, argp, namep)
   );
}

template <typename T>
inline
Thread*
Thread::create_pool(
   MemoryPool<T>& mempool,
   Priority       priority,
   Function       threadf,
   void*          argp,
   const char*    namep
)
{
   return reinterpret_cast<Thread*>(
      Thread_::create_pool(mempool, priority, threadf, argp, namep)
   );
}

inline
Thread&
Thread::self()
{
   return reinterpret_cast<Thread&>(Thread_::self());
}

inline
Thread::Priority
Thread::get_priority()
{
   return Thread_::get_priority();
}

inline
void
Thread::set_priority(
   Priority priority
)
{
   Thread_::set_priority(priority);
}

inline
void
Thread::yield()
{
   Thread_::yield();
}

inline
void
Thread::sleep(
   const Time& delay
)
{
   Thread_::sleep(delay);
}

inline
void
Thread::sleep_until(
   const Time& time
)
{
   Thread_::sleep_until(time);
}

inline
Thread::Return
Thread::sleep()
{
   return Thread_::sleep();
}

inline void
Thread::wake(
   Thread&        thread,
   Thread::Return msg
)
{
   Thread_::wake(thread.impl, msg);
}

inline
void
Thread::exit(
   uint32_t msg
)
{
   Thread_::exit(msg);
}

inline
bool
Thread::join(
   Thread& thread
)
{
   return Thread_::join(thread.impl);
}

inline bool
Thread::should_terminate()
{
   return Thread_::should_terminate();
}

NAMESPACE_CORE_OS_END
