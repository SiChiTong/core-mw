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

const CoreModule::UID&
CoreModule::uid()
{
    return core::hw::UID_::get();
}

const char*
CoreModule::moduleName()
{
    return ModuleConfiguration::MODULE_NAME;
}

uint8_t
CoreModule::moduleID()
{
#ifdef MODULE_ID
    return MODULE_ID & 0xFF;

#else
    return core::hw::UID_::CPU_UID(0) ^ core::hw::UID_::CPU_UID(2);
#endif
}

NAMESPACE_CORE_MW_END
