/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreModule.hpp>

#include <core/os/OS.hpp>
#include <core/hw/UID.hpp>

#include <core/utils/LFSR.hpp>

#include <ModuleConfiguration.hpp>

NAMESPACE_CORE_MW_BEGIN

static CoreModule::Led _led;
static core::utils::LFSR<uint32_t, 0x80000ACDu> _prn(CoreModule::uid());

CoreModule::Led& CoreModule::led = _led;

CoreNodeManager CoreModule::_nodes;
CoreConfigurationManager CoreModule::_configurations;

core::mw::CoreConfigurationStorage&
CoreModule::configurationStorage()
{
    return _coreConfigurationStorage;
}

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

CoreNodeManager&
CoreModule::nodes()
{
    return _nodes;
}

CoreConfigurationManager&
CoreModule::configurations()
{
    return _configurations;
}

CoreModule::CoreModule() {}

uint32_t
CoreModule::getPseudorandom()
{
    return _prn();
}

NAMESPACE_CORE_MW_END
