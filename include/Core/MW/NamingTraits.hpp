/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>

NAMESPACE_CORE_MW_BEGIN

class Middleware;
class Node;
class Topic;
class Transport;


template <typename T>
struct NamingTraits {};


template <>
struct NamingTraits<Middleware> {
	enum {
		MAX_LENGTH = 7
	};
};


template <>
struct NamingTraits<Node> {
	enum {
		MAX_LENGTH = 8
	};
};


template <>
struct NamingTraits<Topic> {
	enum {
		MAX_LENGTH = 16
	};
};


template <>
struct NamingTraits<Transport> {
	enum {
		MAX_LENGTH = 8
	};
};


NAMESPACE_CORE_MW_END
