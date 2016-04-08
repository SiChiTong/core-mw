#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Transport.hpp>
#include <Core/MW/BaseSubscriberQueue.hpp>
#include <Core/MW/TimestampedMsgPtrQueue.hpp>
#include <Core/MW/Mutex.hpp>
#include <Core/MW/MgmtMsg.hpp>
#include <Core/MW/Semaphore.hpp>
#include <Core/MW/Thread.hpp>
#include <Core/MW/MemoryPool.hpp>

#include "rtcan.h"
#include "RTCANPublisher.hpp"
#include "RTCANSubscriber.hpp"


NAMESPACE_CORE_MW_BEGIN

class Message;

class RTCANTransport:
	public Transport
{
private:
	RTCANDriver& rtcan;
	// FIXME to move in pub/sub?
	rtcan_msg_t header_buffer[10];
	MemoryPool<rtcan_msg_t> header_pool;

	BaseSubscriberQueue subp_queue;

	enum {
		MGMT_BUFFER_LENGTH = 4
	};
	MgmtMsg mgmt_msgbuf[MGMT_BUFFER_LENGTH];
	TimestampedMsgPtrQueue::Entry mgmt_msgqueue_buf[MGMT_BUFFER_LENGTH];
	RTCANSubscriber* mgmt_rsub;
	RTCANPublisher*  mgmt_rpub;

public:
	bool
	send(
			Message*         msgp,
			RTCANSubscriber* rsubp
	);

	rtcan_id_t
	topic_id(
			const Topic& topic
	) const;                     // FIXME

	void
	initialize(
			const RTCANConfig& rtcan_config
	);

	void
	fill_raw_params(
			const Topic& topic,
			uint8_t      raw_params[]
	);


private:
	RemotePublisher*
	create_publisher(
			Topic&        topic,
			const uint8_t raw_params[] = NULL
	) const;

	RemoteSubscriber*
	create_subscriber(
			Topic&                        topic,
			TimestampedMsgPtrQueue::Entry queue_buf[], // TODO: remove
			size_t                        queue_length
	) const;


public:
	RTCANTransport(
			RTCANDriver& rtcan
	);
	~RTCANTransport();

private:
	static void
	send_cb(
			rtcan_msg_t& rtcan_msg
	);

	static void
	recv_cb(
			rtcan_msg_t& rtcan_msg
	);

	static void
	free_header(
			rtcan_msg_t& rtcan_msg
	);
};

NAMESPACE_CORE_MW_END
