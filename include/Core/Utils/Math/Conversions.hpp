/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

NAMESPACE_CORE_UTILS_MATH_BEGIN

namespace Conversions {
   template <typename T>
   inline T
   Deg2Rad(
      T deg
   )
   {
      return deg * (Core::Utils::Math::Constants::pi<T>() / (T)(180));
   }

   template <typename T>
   inline T
   Rad2Deg(
      T rad
   )
   {
      return rad * ((T)(180) / Core::Utils::Math::Constants::pi<T>());
   }
}
NAMESPACE_CORE_UTILS_MATH_END
