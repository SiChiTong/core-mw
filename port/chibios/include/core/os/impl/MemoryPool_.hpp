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


class MemoryPool_:
   private core::Uncopyable
{
public:
   typedef ::memgetfunc_t Allocator;

private:
   ::memory_pool_t impl;

public:
   size_t
   get_item_size() const;

   Allocator
   get_allocator() const;

   void*
   alloc_unsafe();

   void
   free_unsafe(
      void* objp
   );

   void*
   alloc();

   void
   free(
      void* objp
   );

   void
   extend(
      void*  arrayp,
      size_t arraylen
   );


   ::memory_pool_t & get_impl();

public:
   MemoryPool_(
      size_t blocklen
   );
   MemoryPool_(
      void*  arrayp,
      size_t length,
      size_t blocklen
   );
   MemoryPool_(
      size_t    blocklen,
      Allocator allocator
   );
   MemoryPool_(
      void*     arrayp,
      size_t    length,
      size_t    blocklen,
      Allocator allocator
   );
};


inline
size_t
MemoryPool_::get_item_size() const
{
   return impl.mp_object_size;
}

inline
MemoryPool_::Allocator
MemoryPool_::get_allocator() const
{
   return impl.mp_provider;
}

inline
void*
MemoryPool_::alloc_unsafe()
{
   return chPoolAllocI(&impl);
}

inline
void
MemoryPool_::free_unsafe(
   void* objp
)
{
   if (objp != NULL) {
      chPoolFreeI(&impl, objp);
   }
}

inline
void*
MemoryPool_::alloc()
{
   return chPoolAlloc(&impl);
}

inline
void
MemoryPool_::free(
   void* objp
)
{
   if (objp != NULL) {
      chPoolFree(&impl, objp);
   }
}

inline
void
MemoryPool_::extend(
   void*  arrayp,
   size_t length
)
{
   chPoolLoadArray(&impl, arrayp, length);
}

inline
  ::memory_pool_t& MemoryPool_::get_impl() {
   return impl;
}


inline
MemoryPool_::MemoryPool_(
   size_t blocklen
)
{
   chPoolObjectInit(&impl, blocklen, reinterpret_cast<Allocator>(NULL));
}

inline
MemoryPool_::MemoryPool_(
   void*  arrayp,
   size_t length,
   size_t blocklen
)
{
   chPoolObjectInit(&impl, blocklen, reinterpret_cast<Allocator>(NULL));
   extend(arrayp, length);
}

inline
MemoryPool_::MemoryPool_(
   size_t    blocklen,
   Allocator allocator
)
{
   chPoolObjectInit(&impl, blocklen, reinterpret_cast<Allocator>(allocator));
}

inline
MemoryPool_::MemoryPool_(
   void*     arrayp,
   size_t    length,
   size_t    blocklen,
   Allocator allocator
)
{
   chPoolObjectInit(&impl, blocklen, reinterpret_cast<Allocator>(allocator));
   extend(arrayp, length);
}

NAMESPACE_CORE_OS_END
