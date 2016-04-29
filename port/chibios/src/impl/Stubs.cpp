/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef SKIP_CORE_STUBS

#include "osal.h"

// disable name demangling
namespace __gnu_cxx {
   void
   __verbose_terminate_handler()
   {
      osalSysHalt("__verbose_terminate_handler.");
   }
}

extern "C" {
#include <stdio.h>
#include <errno.h>

#include "osal.h"

//void *__dso_handle; // -fno-common

   void
   _exit(
      int status
   )
   {
      (void)status;
      osalSysHalt("Unrealized");
   }

   pid_t
   _getpid(
      void
   )
   {
      return 1;
   }

#undef errno
   extern int errno;
   int
   _kill(
      int pid,
      int sig
   )
   {
      (void)pid;
      (void)sig;
      errno = EINVAL;
      return -1;
   }

   void
   _open_r(
      void
   )
   {
      return;
   }

   void
   __cxa_pure_virtual()
   {
      osalSysHalt("Pure virtual function call.");
   }
}

#include <ch.h>


void*
operator new(
   size_t size
)
{
   return chHeapAlloc(NULL, (size > 0) ? size : 1);
}

void*
operator new(
   size_t           size,
     ::memory_heap* heapp
)
{
   return chHeapAlloc(heapp, (size > 0) ? size : 1);
}

void*
operator new [](
   size_t size
)
{
   return chHeapAlloc(NULL, (size > 0) ? size : 1);
}

void*
operator new [](
   size_t           size,
     ::memory_heap* heapp
)
{
   return chHeapAlloc(heapp, (size > 0) ? size : 1);
}

void
operator delete(
   void* objp
)
{
   if (objp != NULL) {
      chHeapFree(objp);
   }
}

void
operator delete(
   void*            objp,
     ::memory_heap* heapp
)
{
   (void)heapp;

   if (objp != NULL) {
      chHeapFree(objp);
   }
}

void
operator delete [](
   void* objp
)
{
   if (objp != NULL) {
      chHeapFree(objp);
   }
}

void
operator delete [](
   void*            objp,
     ::memory_heap* heapp
)
{
   (void)heapp;

   if (objp != NULL) {
      chHeapFree(objp);
   }
}

#endif // ifndef SKIP_CORE_STUBS
