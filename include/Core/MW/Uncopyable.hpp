#pragma once

#include <Core/MW/namespace.hpp>

NAMESPACE_CORE_MW_BEGIN


class Uncopyable
{
private:
	// Must not be defined
	Uncopyable(
			const Uncopyable&
	);

	// Must not be defined
	template <typename T>
	Uncopyable(
			const T&
	);

	// Must not be defined
	template <typename T>
	Uncopyable&
	operator=(
			const T&
	);


protected:
	Uncopyable() {}

	~Uncopyable() {}
};


NAMESPACE_CORE_MW_END
