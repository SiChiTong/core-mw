/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <chprintf.h>
#include <stdarg.h>

NAMESPACE_CORE_MW_BEGIN

class IOStream
{
public:
	virtual int
	printf(
			const char* fmt,
			...
	) = 0;
};

template <class _STREAM>
// TODO:: Must be placed somewhere else, it's a port-specific piece of code!
class IOStream_:
	public Core::MW::IOStream
{
public:
	using STREAM = _STREAM;

	inline int
	printf(
			const char* fmt,
			...
	)
	{
		va_list ap;
		int     formatted_bytes;

		va_start(ap, fmt);
		formatted_bytes = chvprintf(STREAM::stream, fmt, ap);
		va_end(ap);

		return formatted_bytes;
	}
};

NAMESPACE_CORE_MW_END
