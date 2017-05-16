/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreModule.hpp>

#include <core/os/OS.hpp>
#include <core/hw/UID.hpp>

#include <ModuleConfiguration.hpp>

NAMESPACE_CORE_MW_BEGIN

static CoreModule::Led _led;

CoreModule::Led& CoreModule::led = _led;

bool
CoreModule::initialize()
{
    core::os::OS::initialize();

    return true;
}

const void
CoreModule::halt(
    const char* message
)
{
    core::os::OS::halt(message);
}

CoreModule::CoreModule() {}

NAMESPACE_CORE_MW_END
