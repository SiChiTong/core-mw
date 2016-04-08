#pragma once

NAMESPACE_CORE_UTILS_MATH_BEGIN

namespace Conversions {
	template <typename T>
	inline T
	Deg2Rad(
			T deg
	)
	{
		return deg * (Core::Utils::Math::Constants::pi<T>() / (T)(180));
	}

	template <typename T>
	inline T
	Rad2Deg(
			T rad
	)
	{
		return rad * ((T)(180) / Core::Utils::Math::Constants::pi<T>());
	}
}
NAMESPACE_CORE_UTILS_MATH_END
