#include <Core/MW/CoreModule.hpp>

#include "ch.h"
#include "hal.h"

#include <Configuration.hpp>

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

/*
   Led& Board::led() {

   }
 */

NAMESPACE_CORE_MW_END
