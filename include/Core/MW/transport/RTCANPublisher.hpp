#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/RemotePublisher.hpp>
#include <Core/MW/StaticList.hpp>

#include "rtcan.h"

NAMESPACE_CORE_MW_BEGIN


class RTCANPublisher:
	public RemotePublisher
{
public:
	rtcan_msg_t rtcan_header; // FIXME should be private

	RTCANPublisher(
			Transport& transport
	);
	virtual
	~RTCANPublisher();
};


NAMESPACE_CORE_MW_END
