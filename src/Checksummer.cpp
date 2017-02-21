/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Checksummer.hpp>

NAMESPACE_CORE_MW_BEGIN


void
Checksummer::add(
    const uint8_t* chunkp,
    size_t         length
)
{
    while (length-- > 0) {
        add(*chunkp++);
    }
}

NAMESPACE_CORE_MW_END
