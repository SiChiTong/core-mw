#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>

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
