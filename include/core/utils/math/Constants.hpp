/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/utils/math/namespace.hpp>

NAMESPACE_CORE_UTILS_MATH_BEGIN

namespace constants {
template <class T>
inline constexpr T
pi(
   T* = 0
)
{
   return 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651e+00;
}

template <>
inline constexpr float
pi<float>(
   float*
)
{
   return 3.14159265358979f;
}

template <>
inline constexpr double
pi<double>(
   double*
)
{
   return 3.141592653589793238463;
}
}

NAMESPACE_CORE_UTILS_MATH_END
