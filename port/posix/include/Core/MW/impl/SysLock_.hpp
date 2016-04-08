#pragma once

#include <Core/MW/namespace.hpp>

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


private:
	static volatile unsigned exclusion;
};


inline
void
SysLock_::acquire()
{
	while (__sync_lock_test_and_set(&exclusion, 1)) {
		while (exclusion) {}
	}
}

inline
void
SysLock_::release()
{
	__sync_lock_release(&exclusion);
}

NAMESPACE_CORE_MW_END
