/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/StaticQueue.hpp>

NAMESPACE_CORE_MW_BEGIN

class BaseSubscriber;


class BaseSubscriberQueue:
    public StaticQueue<BaseSubscriber>
{
public:
    BaseSubscriberQueue();
};


NAMESPACE_CORE_MW_END
