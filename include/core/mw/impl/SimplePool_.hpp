/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>

NAMESPACE_CORE_MW_BEGIN


class SimplePool_:
   private Uncopyable
{
public:
   struct Header {
      Header* nextp;
   };

private:
   Header* headp;

public:
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
   grow(
      void*  arrayp,
      size_t length,
      size_t blocklen
   );


public:
   SimplePool_();
   SimplePool_(
      void*  arrayp,
      size_t length,
      size_t blocklen
   );
};


NAMESPACE_CORE_MW_END
