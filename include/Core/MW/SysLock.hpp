#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/impl/SysLock_.hpp>

NAMESPACE_CORE_MW_BEGIN


class SysLock:
	private Uncopyable
{
public:
	class Scope:
		private Uncopyable
	{
public:
		Scope()
		{
			SysLock::acquire();
		}

		~Scope()
		{
			SysLock::release();
		}
	};

private:
	SysLock();

public:
	static void
	acquire();

	static void
	release();
};


inline
void
SysLock::acquire()
{
	SysLock_::acquire();
}

inline
void
SysLock::release()
{
	SysLock_::release();
}

NAMESPACE_CORE_MW_END
