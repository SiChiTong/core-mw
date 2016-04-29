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


class Mutex_:
	private Uncopyable
{
private:
	::mutex_t impl;

public:
	void
	initialize();

	void
	acquire_unsafe();

	void
	release_unsafe();

	void
	acquire();

	void
	release();


	::mutex_t & get_impl();

public:
	Mutex_();
	explicit
	Mutex_(
			bool initialize
	);
};


inline
void
Mutex_::initialize()
{
	chMtxObjectInit(&impl);
}

inline
void
Mutex_::acquire_unsafe()
{
	chMtxLockS(&impl);
}

inline
void
Mutex_::release_unsafe()
{
	chMtxUnlockS(&impl);
}

inline
void
Mutex_::acquire()
{
	chMtxLock(&impl);
}

inline
void
Mutex_::release()
{
	chMtxUnlock(&impl);
}

inline
  ::mutex_t& Mutex_::get_impl() {
	return impl;
}


inline
Mutex_::Mutex_()
{
	this->initialize();
}

inline
Mutex_::Mutex_(
		bool initialize
)
{
	if (initialize) {
		this->initialize();
	}
}

NAMESPACE_CORE_MW_END
