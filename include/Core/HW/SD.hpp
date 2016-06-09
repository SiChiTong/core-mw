/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/HW/namespace.hpp>
#include <Core/HW/common.hpp>

#include "hal.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t N>
struct SDDriverTraits {};

#if STM32_SERIAL_USE_USART1
template <>
struct SDDriverTraits<1> {
   static constexpr auto driver = &SD1;
};
#endif

#if STM32_SERIAL_USE_USART2
template <>
struct SDDriverTraits<2> {
   static constexpr auto driver = &SD2;
};
#endif

#if STM32_SERIAL_USE_USART3
template <>
struct SDDriverTraits<3> {
   static constexpr auto driver = &SD3;
};
#endif

#if STM32_SERIAL_USE_USART4
template <>
struct SDDriverTraits<4> {
   static constexpr auto driver = &SD4;
};
#endif

#if STM32_SERIAL_USE_USART5
template <>
struct SDDriverTraits<5> {
   static constexpr auto driver = &SD5;
};
#endif

// --- Aliases -----------------------------------------------------------------

using SD_1 = SDDriverTraits<1>;
using SD_2 = SDDriverTraits<2>;
using SD_3 = SDDriverTraits<3>;
using SD_4 = SDDriverTraits<4>;
using SD_5 = SDDriverTraits<5>;

NAMESPACE_CORE_HW_END
