/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreModule.hpp>

#include "ch.h"
#include "hal.h"

#include <core/hw/IWDG.hpp>
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

void
CoreModule::reset()
{
   core::hw::IWDG_::woof();
}

void
CoreModule::keepAlive()
{
   core::hw::IWDG_::reload();
}

void
CoreModule::disableBootloader()
{
   RTC->BKP0R = 0x55AA55AA; // TODO: wrap it somewhere.
}

void
CoreModule::enableBootloader()
{
   RTC->BKP0R = 0xB0BAFE77; // TODO: wrap it somewhere.
}

CoreModule::CoreModule() {}

const CoreModule::UID&
CoreModule::uid()
{
   return core::hw::UID_::get();
}

/*
   Led& Board::led() {

   }
 */

NAMESPACE_CORE_MW_END
