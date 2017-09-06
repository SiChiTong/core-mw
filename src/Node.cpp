/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Node.hpp>
#include <core/mw/Middleware.hpp>
#include <core/os/Thread.hpp>
#include <core/mw/MgmtMsg.hpp>
#include <core/mw/Publisher.hpp>
#include <core/mw/Subscriber.hpp>

NAMESPACE_CORE_MW_BEGIN


bool
Node::advertise(
    LocalPublisher&       pub,
    const char*           namep,
    const core::os::Time& publish_timeout,
    size_t                msg_size
)
{
    // already advertised
    if (pub.get_topic() != nullptr) {
        return false;
    }

    if (Middleware::instance().advertise(pub, namep, publish_timeout, msg_size)) {
        publishers.link(pub.by_node);
        return true;
    }

    return false;
}

bool
Node::subscribe(
    LocalSubscriber& sub,
    const char*      namep,
    Message          msgpool_buf[],
    size_t           msg_size
)
{
    // already subscribed
    if (sub.get_topic() != nullptr) {
        return false;
    }

    sub.nodep = this;
    int index = subscribers.count();
    subscribers.link(sub.by_node);
    CORE_ASSERT(index >= 0);
    CORE_ASSERT(index <= static_cast<int>(core::os::SpinEvent::MAX_INDEX));
    sub.event_index = static_cast<uint_least8_t>(index);

    if (!Middleware::instance().subscribe(sub, namep, msgpool_buf, sub.get_queue_length(), msg_size)) {
        subscribers.unlink(sub.by_node);
        return false;
    }

    return true;
} // Node::subscribe

bool
Node::spin(
    const core::os::Time& timeout,
    void*                 context
)
{
    core::os::SpinEvent::Mask mask;

    mask = event.wait(timeout);

    if (mask == 0) {
        return false;
    }

    core::os::Time dummy_timestamp;
    core::os::SpinEvent::Mask bit = 1;

    for (StaticList<LocalSubscriber>::Iterator i = subscribers.begin(); i != subscribers.end() && mask != 0; bit <<= 1, ++i) {
        if ((mask & bit) != 0) {
            mask &= ~bit;
            const LocalSubscriber::CallbackFunction* callback = i->get_callback();

            if (callback != nullptr) {
                Message* msgp;

                while (i->fetch(msgp, dummy_timestamp)) {
                    (*callback)(*msgp, context);
                    i->release(*msgp);
                }
            }
        }
    }

    return true;
} // Node::spin

Node::Node(
    const char* namep,
    bool        enabled
)
    :
    event(enabled ? &core::os::Thread::self() : nullptr),
    namep(namep),
    by_middleware(*this)
{
    CORE_ASSERT(is_identifier(namep, NamingTraits<Node>::MAX_LENGTH));

    Middleware::instance().add(*this);
}

Node::~Node()
{
    Middleware::instance().confirm_stop(*this);
}

NAMESPACE_CORE_MW_END
