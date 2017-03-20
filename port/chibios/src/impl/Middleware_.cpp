/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Middleware.hpp>
#include <core/mw/CoreModule.hpp>
#include <core/hw/IWDG.hpp>

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
