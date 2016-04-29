/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#include <Core/MW/namespace.hpp>
#include <Core/MW/BasePublisher.hpp>
#include <Core/MW/Topic.hpp>
#include <Core/MW/Message.hpp>

NAMESPACE_CORE_MW_BEGIN


bool
BasePublisher::publish_unsafe(
		Message& msg
)
{
	CORE_ASSERT(topicp != NULL);

	msg.acquire_unsafe();

	Time now = Time::now();
	bool success;
	success = topicp->notify_locals_unsafe(msg, now);
	success = topicp->notify_remotes_unsafe(msg, now) && success;

	if (!msg.release_unsafe()) {
		topicp->free_unsafe(msg);
	}

	return success;
}

bool
BasePublisher::publish_locally_unsafe(
		Message& msg
)
{
	CORE_ASSERT(topicp != NULL);

	msg.acquire_unsafe();
	bool success = topicp->notify_locals_unsafe(msg, Time::now());

	if (!msg.release_unsafe()) {
		topicp->free_unsafe(msg);
	}

	return success;
}

bool
BasePublisher::publish_remotely_unsafe(
		Message& msg
)
{
	CORE_ASSERT(topicp != NULL);

	msg.acquire_unsafe();
	bool success = topicp->notify_remotes_unsafe(msg, Time::now());

	if (!msg.release_unsafe()) {
		topicp->free_unsafe(msg);
	}

	return success;
}

bool
BasePublisher::publish(
		Message& msg
)
{
	CORE_ASSERT(topicp != NULL);

	msg.acquire();

	Time now = Time::now();
	bool success;
	success = topicp->notify_locals(msg, now);
	success = topicp->notify_remotes(msg, now) && success;

	if (!msg.release()) {
		topicp->free(msg);
	}

	return success;
}

BasePublisher::BasePublisher()
	:
	topicp(NULL)
{}


BasePublisher::~BasePublisher() {}


NAMESPACE_CORE_MW_END
