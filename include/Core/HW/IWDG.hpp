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

class IWDG_
{
public:
   static inline void
   reload()
   {
      IWDG->KR = 0xAAAA; // reload
   }

   static inline void
   woof()
   {
      port_disable(); // Make sure not to be disturbed...

      IWDG->KR  = 0x5555; // enable access
      IWDG->PR  = 0; // /4
      IWDG->RLR = 0x0; // minimum (circa 125us?)
      IWDG->KR  = 0xCCCC; // start watchdog

      while (1) {
         // wait the unavoidable fate...
      }
   }
};

NAMESPACE_CORE_HW_END
