#include <Core/MW/namespace.hpp>
#include <Core/MW/Time.hpp>
#include <ch.h>

NAMESPACE_CORE_MW_BEGIN


Time
Time::now()
{
	register Type t = chVTGetSystemTimeX() / (CH_CFG_ST_FREQUENCY / 1000L);

	return ms((t == INFINITEN.raw) ? (t + 1) :
			(t == INFINITE.raw) ? (t - 1) : t);
}

NAMESPACE_CORE_MW_END
