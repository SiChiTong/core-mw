/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Transport.hpp>
#include <core/mw/Message.hpp>
#include <core/mw/Middleware.hpp>
#include <core/mw/RemotePublisher.hpp>
#include <core/mw/RemoteSubscriber.hpp>
#include <core/mw/TimestampedMsgPtrQueue.hpp>
#include <core/os/ScopedLock.hpp>

NAMESPACE_CORE_MW_BEGIN


bool
Transport::touch_publisher(
    Topic&        topic,
    const uint8_t raw_params[]
)
{
    core::os::ScopedLock<core::os::Mutex> lock(publishers_lock);

    // Check if the remote publisher already exists
    RemotePublisher* pubp;

    pubp = publishers.find_first(BasePublisher::has_topic, topic.get_name());

    if (pubp != nullptr) {
        return true;
    }

    // Create a new remote publisher
    pubp = create_publisher(topic, raw_params);

    if (pubp == nullptr) {
        return false;
    }

    pubp->notify_advertised(topic);
    topic.advertise(*pubp, core::os::Time::INFINITE);
    publishers.link(pubp->by_transport);

    return true;
} // Transport::touch_publisher

bool
Transport::touch_subscriber(
    Topic&  topic,
    size_t  queue_length,
    uint8_t raw_params[]
)
{
    core::os::ScopedLock<core::os::Mutex> lock(subscribers_lock);

    // Check if the remote subscriber already exists
    RemoteSubscriber* subp;

    subp = subscribers.find_first(BaseSubscriber::has_topic, topic.get_name());

    if (subp != nullptr) {
        fill_raw_params(topic, raw_params);
        return true;
    }

    // Create a new remote subscriber
    Message* msgpool_bufp = nullptr;
    TimestampedMsgPtrQueue::Entry* queue_bufp = nullptr;

    msgpool_bufp = reinterpret_cast<Message*>(new uint8_t[topic.get_type_size() * queue_length]); // TODO check with M0

    if (msgpool_bufp != nullptr) {
        queue_bufp = new TimestampedMsgPtrQueue::Entry[queue_length];

        if (queue_bufp != nullptr) {
            subp = create_subscriber(topic, queue_bufp, queue_length);

            if (subp != nullptr) {
                topic.extend_pool(msgpool_bufp, queue_length);
                subp->notify_subscribed(topic);
                topic.subscribe(*subp, queue_length);
                subscribers.link(subp->by_transport);
                fill_raw_params(topic, raw_params);
                return true;
            }
        }
    }

    if ((msgpool_bufp == nullptr) || (queue_bufp == nullptr) || (subp == nullptr)) {
        delete [] queue_bufp;
        delete [] msgpool_bufp;
    }

    return false;
} // Transport::touch_subscriber

bool
Transport::advertise_cb(
    Topic&        topic,
    const uint8_t raw_params[]
)
{
    return touch_publisher(topic, raw_params);
}

bool
Transport::subscribe_cb(
    Topic&  topic,
    size_t  queue_length,
    uint8_t raw_params[]
)
{
    return touch_subscriber(topic, queue_length, raw_params);
}

bool
Transport::advertise(
    RemotePublisher&      pub,
    const char*           namep,
    const core::os::Time& publish_timeout,
    size_t                type_size
)
{
    publishers_lock.acquire();

    if (Middleware::instance().advertise(pub, namep, publish_timeout, type_size)) {
        publishers.link(pub.by_transport);
        publishers_lock.release();
        return true;
    } else {
        publishers_lock.release();
        return false;
    }
}

bool
Transport::subscribe(
    RemoteSubscriber& sub,
    const char*       namep,
    Message           msgpool_buf[],
    size_t            msgpool_buflen,
    size_t            type_size
)
{
    subscribers_lock.acquire();

    if (Middleware::instance().subscribe(sub, namep, msgpool_buf, msgpool_buflen, type_size)) {
        subscribers.link(sub.by_transport);
        subscribers_lock.release();
        return true;
    } else {
        subscribers_lock.release();
        return false;
    }
}

void
Transport::fill_raw_params(
    const Topic& topic,
    uint8_t      raw_params[]
)
{
    (void)topic;
    (void)raw_params;

    memset(raw_params, 0xCC, MgmtMsg::PubSub::MAX_RAW_PARAMS_LENGTH);
}

Transport::Transport(
    const char* namep
)
    :
    namep(namep),
    by_middleware(*this)
{
    CORE_ASSERT(is_identifier(namep, NamingTraits<Transport>::MAX_LENGTH));
}

Transport::~Transport() {}


NAMESPACE_CORE_MW_END
