/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/common.hpp>

NAMESPACE_CORE_MW_BEGIN


bool
is_identifier(
   const char* namep
);

bool
is_identifier(
   const char* namep,
   size_t      max_length
);


NAMESPACE_CORE_MW_END
