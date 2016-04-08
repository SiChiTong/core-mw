#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Time.hpp>
#include <ch.h>

NAMESPACE_CORE_MW_BEGIN


class Condition_:
	private Uncopyable
{
private:
	::condition_variable_t impl;

public:
	void
	initialize();

	void
	signal_unsafe();

	void
	broadcast_unsafe();

	void
	wait_unsafe();

	bool
	wait_unsafe(
			const Time& timeout
	);

	void
	signal();

	void
	broadcast();

	void
	wait();

	bool
	wait(
			const Time& timeout
	);


	::condition_variable_t & get_impl();

public:
	Condition_();
	explicit
	Condition_(
			bool initialize
	);
};


inline
void
Condition_::initialize()
{
	chCondObjectInit(&impl);
}

inline
void
Condition_::signal_unsafe()
{
	chCondSignalI(&impl);
}

inline
void
Condition_::broadcast_unsafe()
{
	chCondBroadcastI(&impl);
}

inline
void
Condition_::wait_unsafe()
{
	chCondWaitS(&impl);
}

inline
bool
Condition_::wait_unsafe(
		const Time& timeout
)
{
	return chCondWaitTimeoutS(&impl, US2ST(timeout.to_us_raw())) == MSG_OK;
}

inline
void
Condition_::signal()
{
	chCondSignal(&impl);
}

inline
void
Condition_::broadcast()
{
	chCondBroadcast(&impl);
}

inline
void
Condition_::wait()
{
	chCondWait(&impl);
}

inline
bool
Condition_::wait(
		const Time& timeout
)
{
	return chCondWaitTimeout(&impl, US2ST(timeout.to_us_raw())) == MSG_OK;
}

inline
  ::condition_variable_t& Condition_::get_impl() {
	return impl;
}


inline
Condition_::Condition_()
{
	initialize();
}

inline
Condition_::Condition_(
		bool initialize
)
{
	if (initialize) {
		this->initialize();
	}
}

NAMESPACE_CORE_MW_END
