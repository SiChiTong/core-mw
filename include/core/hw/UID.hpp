/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/hw/namespace.hpp>
#include <core/hw/common.hpp>
#include <core/Array.hpp>

#include "hal.h"

NAMESPACE_CORE_HW_BEGIN

using UID = core::Array<uint8_t, 12>;

class UID_
{
public:
   static constexpr uint8_t
   CPU_UID(
      std::size_t i
   )
   {
      return (i < 12) ? *((uint8_t*)(0x1FFFF7AC + i)) : 0xFF; // TODO: do it...
   }

   inline static const UID&
   get()
   {
      return _uid;
   }

private:
   static const UID _uid;
};

NAMESPACE_CORE_HW_END
