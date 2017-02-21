/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/MessagePtrQueue.hpp>
#include <core/mw/StaticList.hpp>
#include <core/os/MemoryPool.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_MW_BEGIN

class Topic;
class Message;

class BaseSubscriber:
    private core::Uncopyable
{
private:
    Topic* topicp;

public:
    Topic*
    get_topic() const;

    virtual size_t
    get_queue_length() const = 0;

    void
    notify_subscribed(
        Topic& topic
    );

    virtual bool
    notify_unsafe(
        Message&              msg,
        const core::os::Time& timestamp
    ) = 0;

    virtual bool
    fetch_unsafe(
        Message*&       msgp,
        core::os::Time& timestamp
    ) = 0;

    bool
    release_unsafe(
        Message& msg
    );

    virtual bool
    notify(
        Message&              msg,
        const core::os::Time& timestamp,
        bool                  mustReschedule = false
    ) = 0;

    virtual bool
    fetch(
        Message*&       msgp,
        core::os::Time& timestamp
    ) = 0;

    bool
    release(
        Message& msg
    );


protected:
    BaseSubscriber();
    virtual
    ~BaseSubscriber() = 0;

public:
    static bool
    has_topic(
        const BaseSubscriber& sub,
        const char*           namep
    );
};


inline
Topic*
BaseSubscriber::get_topic() const
{
    return topicp;
}

inline
void
BaseSubscriber::notify_subscribed(
    Topic& topic
)
{
    CORE_ASSERT(topicp == NULL);

    topicp = &topic;
}

NAMESPACE_CORE_MW_END
