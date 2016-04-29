/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#include <Core/MW/namespace.hpp>
#include <Core/MW/Bootloader.hpp>

#if CORE_USE_BOOTLOADER
NAMESPACE_CORE_MW_BEGIN


uint8_t*
Bootloader::reserve_ram(
		size_t length
)
{
	return reinterpret_cast<uint8_t*>(chCoreReserve(length));
}

uint8_t*
Bootloader::unreserve_ram(
		size_t length
)
{
	return reinterpret_cast<uint8_t*>(chCoreUnreserve(length));
}

NAMESPACE_CORE_MW_END
#endif // CORE_USE_BOOTLOADER
