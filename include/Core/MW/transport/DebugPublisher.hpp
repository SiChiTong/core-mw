#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/RemotePublisher.hpp>
#include <Core/MW/StaticList.hpp>

NAMESPACE_CORE_MW_BEGIN


class DebugPublisher:
	public RemotePublisher
{
public:
	DebugPublisher(
			Transport& transport
	);
	~DebugPublisher();
};


NAMESPACE_CORE_MW_END
