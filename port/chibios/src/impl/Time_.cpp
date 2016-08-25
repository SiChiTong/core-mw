/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/os/namespace.hpp>
#include <core/os/Time.hpp>
#include <ch.h>

NAMESPACE_CORE_OS_BEGIN


Time
Time::now()
{
   register Type t = chVTGetSystemTimeX() / (CH_CFG_ST_FREQUENCY / 1000L);

   return ms((t == INFINITEN.raw) ? (t + 1) :
             (t == INFINITE.raw) ? (t - 1) : t);
}

uint32_t
Time::ticks() const
{
   return ((systime_t)(((((uint64_t)(raw)) * ((uint64_t)CH_CFG_ST_FREQUENCY)) + 999999ULL) / 1000000ULL));
}

NAMESPACE_CORE_OS_END
