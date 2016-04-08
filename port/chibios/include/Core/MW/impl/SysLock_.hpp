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
