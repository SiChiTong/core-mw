/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_OS_BEGIN


const Time Time::IMMEDIATE(0);
const Time Time::INFINITE(MAX_US + 1);
const Time Time::INFINITEN(MIN_US - 1);


NAMESPACE_CORE_OS_END
