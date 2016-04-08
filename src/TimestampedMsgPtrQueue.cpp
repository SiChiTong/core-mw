#include <Core/MW/namespace.hpp>
#include <Core/MW/TimestampedMsgPtrQueue.hpp>

NAMESPACE_CORE_MW_BEGIN


TimestampedMsgPtrQueue::TimestampedMsgPtrQueue(
		Entry  array[],
		size_t length
)
	:
	impl(array, length)
{}


NAMESPACE_CORE_MW_END
