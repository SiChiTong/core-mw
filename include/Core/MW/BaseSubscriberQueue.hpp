/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/StaticQueue.hpp>

NAMESPACE_CORE_MW_BEGIN

class BaseSubscriber;


class BaseSubscriberQueue:
   public StaticQueue<BaseSubscriber>
{
public:
   BaseSubscriberQueue();
};


NAMESPACE_CORE_MW_END
