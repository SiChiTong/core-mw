/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/StaticList.hpp>
#include <core/mw/TimestampedMsgPtrQueue.hpp>
#include <core/os/Mutex.hpp>
#include <core/mw/NamingTraits.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;

class Topic;
class RemotePublisher;
class RemoteSubscriber;


class Transport:
    private core::Uncopyable
{
    friend class Middleware;

protected:
    const char* namep;
    StaticList<RemotePublisher>  publishers;
    StaticList<RemoteSubscriber> subscribers;
    core::os::Mutex publishers_lock;
    core::os::Mutex subscribers_lock;

private:
    mutable StaticList<Transport>::Link by_middleware;

public:
    const char*
    get_name() const;


    const StaticList<RemotePublisher>&
    get_publishers() const;


    const StaticList<RemoteSubscriber>&
    get_subscribers() const;

    virtual void
    fill_raw_params(
        const Topic& topic,
        uint8_t      raw_params[]
    );


protected:
    bool
    touch_publisher(
        Topic&        topic,
        const uint8_t raw_params[] = NULL
    );

    bool
    touch_subscriber(
        Topic&  topic,
        size_t  queue_length,
        uint8_t raw_params[] = NULL
    );

    bool
    advertise_cb(
        Topic&        topic,
        const uint8_t raw_params[] = NULL
    );

    bool
    subscribe_cb(
        Topic&  topic,
        size_t  queue_length,
        uint8_t raw_params[] = NULL
    );

    bool
    advertise(
        RemotePublisher&      pub,
        const char*           namep,
        const core::os::Time& publish_timeout,
        size_t                type_size
    );

    bool
    subscribe(
        RemoteSubscriber& sub,
        const char*       namep,
        Message           msgpool_buf[],
        size_t            msgpool_buflen,
        size_t            type_size
    );


    template <typename MessageType>
    bool
    advertise(
        RemotePublisher&      pub,
        const char*           namep,
        const core::os::Time& publish_timeout
    );


    template <typename MessageType>
    bool
    subscribe(
        RemoteSubscriber& sub,
        const char*       namep,
        MessageType       msgpool_buf[],
        size_t            msgpool_buflen
    );

    virtual RemotePublisher*
    create_publisher(
        Topic&        topic,
        const uint8_t raw_params[] = NULL
    )
    const = 0;

    virtual RemoteSubscriber*
    create_subscriber(
        Topic&                        topic,
        TimestampedMsgPtrQueue::Entry queue_buf[],
        size_t                        queue_length
    ) const = 0;


protected:
    Transport(
        const char* namep
    );
    virtual
    ~Transport() = 0;

public:
    static bool
    has_name(
        const Transport& transport,
        const char*      namep
    );
};


NAMESPACE_CORE_MW_END

#include <core/mw/Message.hpp>

NAMESPACE_CORE_MW_BEGIN


inline
const char*
Transport::get_name() const
{
    return namep;
}

inline
const StaticList<RemotePublisher>&
Transport::get_publishers() const
{
    return publishers;
}

inline
const StaticList<RemoteSubscriber>&
Transport::get_subscribers() const
{
    return subscribers;
}

template <typename MessageType>
inline
bool
Transport::advertise(
    RemotePublisher&      pub,
    const char*           namep,
    const core::os::Time& publish_timeout
)
{
    static_cast_check<MessageType, Message>();
    return advertise(pub, namep, publish_timeout, sizeof(MessageType));
}

template <typename MessageType>
inline
bool
Transport::subscribe(
    RemoteSubscriber& sub,
    const char*       namep,
    MessageType       msgpool_buf[],
    size_t            msgpool_buflen
)
{
    static_cast_check<MessageType, Message>();
    return subscribe(sub, namep, msgpool_buf, msgpool_buflen, sizeof(MessageType));
}

inline
bool
Transport::has_name(
    const Transport& transport,
    const char*      namep
)
{
    return namep != NULL && 0 == strncmp(transport.get_name(), namep, NamingTraits<Transport>::MAX_LENGTH);
}

NAMESPACE_CORE_MW_END
