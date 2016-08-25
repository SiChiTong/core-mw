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
   if (raw == INFINITE) {
      return TIME_INFINITE;
   } else if (raw <= IMMEDIATE) {
      return TIME_IMMEDIATE;
   } else {
      Type ticks = raw / (1000000 / CH_CFG_ST_FREQUENCY);
      return ticks; // TODO: decide what to do with TIME_INFINITE
   }
}

Time
Time::hz(
   const float hertz
)
{
   if (hertz <= 0) {
      return Time::INFINITE;
   }

   // TODO: decide about assert(hertz <= CH_CFG_ST_FREQUENCY)
   CORE_ASSERT((1000000.0f / hertz) <= ((float)MAX_US)); // Make sure we can represent it

   return Time(static_cast<Type>(1000000.0f / hertz));
}

NAMESPACE_CORE_OS_END
