/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreModule.hpp>

#include "ch.h"
#include "hal.h"

#include <core/hw/UID.hpp>

#include <ModuleConfiguration.hpp>

NAMESPACE_CORE_MW_BEGIN

static CoreModule::Led _led;

CoreModule::Led& CoreModule::led = _led;

bool
CoreModule::initialize()
{
    return true;
}

const void
CoreModule::halt(
    const char* message
)
{
    osalSysHalt(message);
}

CoreModule::CoreModule() {}

const CoreModule::UID&
CoreModule::uid()
{
    return core::hw::UID_::get();
}

NAMESPACE_CORE_MW_END
