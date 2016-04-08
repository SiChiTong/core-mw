#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>

NAMESPACE_CORE_MW_BEGIN


template <typename Lockable>
class ScopedLock:
	private Uncopyable
{
private:
	Lockable& lock;

public:
	ScopedLock(
			Lockable& lock
	);
	~ScopedLock();
};


template <typename Lockable>
inline
ScopedLock<Lockable>::ScopedLock(
		Lockable& lock
)
	:
	lock(lock)
{
	this->lock.acquire();
}

template <typename Lockable>
inline
ScopedLock<Lockable>::~ScopedLock()
{
	this->lock.release();
}

NAMESPACE_CORE_MW_END
