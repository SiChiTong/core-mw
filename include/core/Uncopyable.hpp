/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/namespace.hpp>

NAMESPACE_CORE_BEGIN


class Uncopyable
{
private:
   // Must not be defined
   Uncopyable(
      const Uncopyable&
   );

   // Must not be defined
   template <typename T>
   Uncopyable(
      const T&
   );

   // Must not be defined
   template <typename T>
   Uncopyable&
   operator=(
      const T&
   );


protected:
   Uncopyable() {}

   ~Uncopyable() {}
};


NAMESPACE_CORE_END
