/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <ch.h>

NAMESPACE_CORE_MW_BEGIN


class SysLock_:
	private Uncopyable
{
private:
	SysLock_();

public:
	static void
	acquire();

	static void
	release();
};


inline
void
SysLock_::acquire()
{
	chSysLock();
}

inline
void
SysLock_::release()
{
	chSysUnlock();
}

NAMESPACE_CORE_MW_END
