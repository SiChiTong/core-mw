/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <Core/MW/namespace.hpp>
#include <Core/MW/Middleware.hpp>
#include <Core/MW/CoreModule.hpp>
#include <Core/HW/IWDG.hpp>

#include "ch.h"
#include "hal.h"

NAMESPACE_CORE_MW_BEGIN

void
Middleware::reboot()
{
   CoreModule::reset();
} // Middleware::reboot

void
Middleware::preload_bootloader_mode(
   bool enable
)
{
   if (enable) {
      CoreModule::enableBootloader();
   } else {
      CoreModule::disableBootloader();
   }
}

NAMESPACE_CORE_MW_END
