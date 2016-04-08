#include <Core/MW/namespace.hpp>
#include <Core/MW/MessagePtrQueue.hpp>

NAMESPACE_CORE_MW_BEGIN


MessagePtrQueue::MessagePtrQueue(
		Message* arrayp[],
		size_t   length
)
	:
	ArrayQueue<Message*>(arrayp, length)
{}


NAMESPACE_CORE_MW_END
