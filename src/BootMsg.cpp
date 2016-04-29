/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <Core/MW/namespace.hpp>
#include <Core/MW/BootMsg.hpp>
#include <cstring>

NAMESPACE_CORE_MW_BEGIN


uint8_t
IhexRecord::compute_checksum() const
{
   uint8_t cs = count + static_cast<uint8_t>(offset >> 8)
                + static_cast<uint8_t>(offset & 0xFF) + type;

   for (uint8_t i = 0; i < count; ++i) {
      cs += data[i];
   }

   return ~cs + 1;
}

NAMESPACE_CORE_MW_END
