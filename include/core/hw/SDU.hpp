/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>

#include "hal.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t N>
struct SDUDriverTraits {};

#if HAL_USE_SERIAL_USB
template <>
struct SDUDriverTraits<1> {
   static constexpr auto driver = &SDU1;
};
#endif

// --- Aliases -----------------------------------------------------------------

using SDU_1 = SDUDriverTraits<1>;

NAMESPACE_CORE_HW_END
