/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include <functional>

template <typename T>
struct Callback;

template <typename Ret, typename ... Params>
struct Callback<Ret(Params ...)> {
	template <typename ... Args>
	static Ret
	callback(
			Args ... args
	)
	{
		return func(args ...);
	}

	static std::function<Ret(Params ...)> func;
};

template <typename Ret, typename ... Params>
std::function<Ret(Params ...)> Callback<Ret(Params ...)>::func;
