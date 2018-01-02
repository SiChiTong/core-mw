/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/MessagePtrQueue.hpp>

NAMESPACE_CORE_MW_BEGIN


MessagePtrQueue::MessagePtrQueue(
    Message* arrayp[],
    size_t   length
)
    :
    ArrayQueue<Message*>(arrayp, length)
{}


NAMESPACE_CORE_MW_END
