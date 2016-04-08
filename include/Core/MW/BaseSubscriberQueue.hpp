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
