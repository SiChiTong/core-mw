/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

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
