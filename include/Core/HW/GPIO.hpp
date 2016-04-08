#pragma once

#include <Core/HW/namespace.hpp>
#include <Core/HW/common.hpp>

#include "hal.h"

NAMESPACE_CORE_HW_BEGIN

template <std::size_t N>
struct GPIOPortTraits {};

template <>
struct GPIOPortTraits<0> {
	static constexpr auto gpio = GPIOA;
};

template <>
struct GPIOPortTraits<1> {
	static constexpr auto gpio = GPIOB;
};

template <>
struct GPIOPortTraits<2> {
	static constexpr auto gpio = GPIOC;
};

template <>
struct GPIOPortTraits<3> {
	static constexpr auto gpio = GPIOD;
};

template <>
struct GPIOPortTraits<4> {
	static constexpr auto gpio = GPIOE;
};

template <>
struct GPIOPortTraits<5> {
	static constexpr auto gpio = GPIOF;
};

struct Pad {
	virtual void
	set() = 0;

	virtual void
	clear() = 0;

	virtual void
	toggle() = 0;

	virtual void
	write(
			unsigned on
	) = 0;
};

template <class _GPIO, std::size_t _PAD>
struct Pad_:
	public Pad {
	using GPIO = _GPIO;
	inline void
	set()
	{
		palSetPad(GPIO::gpio, _PAD);
	}

	inline void
	clear()
	{
		palClearPad(GPIO::gpio, _PAD);
	}

	inline void
	toggle()
	{
		palTogglePad(GPIO::gpio, _PAD);
	}

	inline void
	write(
			unsigned on
	)
	{
		palWritePad(GPIO::gpio, _PAD, on);
	}
};

// --- Aliases -----------------------------------------------------------------

using GPIO_A = GPIOPortTraits<0>;
using GPIO_B = GPIOPortTraits<1>;
using GPIO_C = GPIOPortTraits<2>;
using GPIO_D = GPIOPortTraits<3>;
using GPIO_E = GPIOPortTraits<4>;
using GPIO_F = GPIOPortTraits<5>;

NAMESPACE_CORE_HW_END
