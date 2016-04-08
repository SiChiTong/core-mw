#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/BasePublisher.hpp>
#include <Core/MW/StaticList.hpp>

NAMESPACE_CORE_MW_BEGIN


class LocalPublisher:
	public BasePublisher
{
	friend class Node;

private:
	mutable StaticList<LocalPublisher>::Link by_node;

protected:
	LocalPublisher();
	virtual
	~LocalPublisher() = 0;
};


NAMESPACE_CORE_MW_END
