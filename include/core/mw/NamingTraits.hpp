/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>

NAMESPACE_CORE_MW_BEGIN

class Middleware;
class Node;
class Topic;
class Transport;
class CoreConfigurableBase;

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

// Make sure MAX_LENGTH % 4 == 0
template <>
struct NamingTraits<CoreConfigurableBase> {
    enum {
        MAX_LENGTH = 16
    };
};

NAMESPACE_CORE_MW_END
