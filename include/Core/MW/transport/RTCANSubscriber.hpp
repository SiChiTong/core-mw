#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/RemoteSubscriber.hpp>
#include <Core/MW/TimestampedMsgPtrQueue.hpp>
#include <Core/MW/StaticList.hpp>
#include <Core/MW/BaseSubscriberQueue.hpp>
//#include <ch.h>

#include "rtcan.h"

NAMESPACE_CORE_MW_BEGIN

class Message;
class RTCANTransport;


class RTCANSubscriber:
	public RemoteSubscriber
{
	friend class RTCANTransport;

private:
	rtcan_id_t rtcan_id;

public:
	bool
	fetch_unsafe(
			Message*& msgp,
			Time&     timestamp
	);

	bool
	notify_unsafe(
			Message&    msg,
			const Time& timestamp
	);

	bool
	fetch(
			Message*& msgp,
			Time&     timestamp
	);

	bool
	notify(
			Message&    msg,
			const Time& timestamp
	);

	size_t
	get_queue_length() const;


public:
	RTCANSubscriber(
			RTCANTransport&               transport,
			TimestampedMsgPtrQueue::Entry queue_buf[],
			size_t                        queue_length
	);
	virtual
	~RTCANSubscriber();
};

inline
bool
RTCANSubscriber::notify(
		Message&    msg,
		const Time& timestamp
)
{
	SysLock::acquire();
	bool success = notify_unsafe(msg, timestamp);
	SysLock::release();

	return success;
}

inline
bool
RTCANSubscriber::fetch(
		Message*& msgp,
		Time&     timestamp
)
{
	SysLock::acquire();
	bool success = fetch_unsafe(msgp, timestamp);
	SysLock::release();

	return success;
}

NAMESPACE_CORE_MW_END
