/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/LocalSubscriber.hpp>
#include <core/mw/Node.hpp>

NAMESPACE_CORE_MW_BEGIN


bool
LocalSubscriber::fetch(
    Message*& msgp
)
{
    core::os::SysLock::acquire();

    if (msgp_queue.fetch_unsafe(msgp)) {
        core::os::SysLock::release();
        return true;
    } else {
        core::os::SysLock::release();
        return false;
    }
}

bool
LocalSubscriber::fetch(
    Message*&       msgp,
    core::os::Time& timestamp
)
{
    core::os::SysLock::acquire();

    if (msgp_queue.fetch_unsafe(msgp)) {
        core::os::SysLock::release();
        timestamp = core::os::Time::now();
        return true;
    } else {
        core::os::SysLock::release();
        return false;
    }
}

bool
LocalSubscriber::notify(
    Message&              msg,
    const core::os::Time& timestamp,
    bool                  mustReschedule
)
{
    (void)timestamp;
    core::os::SysLock::acquire();

    if (msgp_queue.post_unsafe(&msg)) {
        nodep->notify_unsafe(event_index, mustReschedule);
        core::os::SysLock::release();
        return true;
    } else {
        core::os::SysLock::release();
        return false;
    }
}

LocalSubscriber::LocalSubscriber(
    Message*          queue_buf[],
    size_t            queue_length,
    CallbackFunction* callback
)
    :
    BaseSubscriber(),
    nodep(nullptr),
    callback(callback),
    msgp_queue(queue_buf, queue_length),
    event_index(~0),
    by_node(*this),
    by_topic(*this)
{
    CORE_ASSERT(queue_buf != nullptr);
    CORE_ASSERT(queue_length > 0);
}

LocalSubscriber::~LocalSubscriber() {}

NAMESPACE_CORE_MW_END
