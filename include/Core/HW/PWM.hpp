/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include "hal.h"

#include <Core/HW/namespace.hpp>
#include <Core/HW/common.hpp>
#include <Core/HW/GPIO.hpp>

NAMESPACE_CORE_HW_BEGIN

template <std::size_t E>
struct PWMDriverTraits {};

template <>
struct PWMDriverTraits<1> {
	static constexpr auto driver = &PWMD1;
};

class PWMChannel
{
public:
	using CountDataType = pwmcnt_t;

	virtual void
	stop() = 0;

	virtual void
	enable() = 0;

	virtual void
	disable() = 0;

	virtual bool
	set(
			CountDataType value
	) = 0;
};

template <class _PWM, int _CHANNEL>
class PWMChannel_:
	public PWMChannel
{
public:
	using PWM = _PWM;
	const int CHANNEL = _CHANNEL;

public:
	inline void
	enable()
	{
		::pwmEnableChannel(PWM::driver, CHANNEL, 0);
	}

	inline void
	disable()
	{
		::pwmDisableChannel(PWM::driver, CHANNEL);
	}

	inline void
	stop()
	{
		::pwmStop(PWM::driver);
	}

	inline bool
	set(
			CountDataType value
	)
	{
		::pwmEnableChannel(PWM::driver, CHANNEL, value);
		return true;
	}
};

// --- Aliases -----------------------------------------------------------------

using PWM_1 = PWMDriverTraits<1>;

NAMESPACE_CORE_HW_END
