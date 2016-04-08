#include <Core/MW/namespace.hpp>
#include <Core/MW/Checksummer.hpp>

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
