/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/Message.hpp>
#include <core/mw/MgmtMsg.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;
class Topic;


class BasePublisher:
    private core::Uncopyable
{
private:
    Topic* topicp;

public:
    Topic*
    get_topic() const;

    void
    notify_advertised(
        Topic& topic
    );

    bool
    alloc_unsafe(
        Message*& msgp
    );

    bool
    publish_unsafe(
        Message& msg
    );

    bool
    publish_locally_unsafe(
        Message& msg
    );

    bool
    publish_remotely_unsafe(
        Message& msg
    );

    bool
    alloc(
        Message*& msgp
    );

    bool
    publish(
        Message& msg,
        bool     mustReschedule = false
    );

    bool
    publish_locally(
        Message& msg,
        bool     mustReschedule = false
    );

    bool
    publish_remotely(
        Message& msg
    );


protected:
    BasePublisher();
    virtual
    ~BasePublisher() = 0;

public:
    static bool
    has_topic(
        const BasePublisher& pub,
        const char*          namep
    );
};


NAMESPACE_CORE_MW_END

#include <core/mw/Topic.hpp>

NAMESPACE_CORE_MW_BEGIN


inline
Topic*
BasePublisher::get_topic() const
{
    return topicp;
}

inline
void
BasePublisher::notify_advertised(
    Topic& topic
)
{
    CORE_ASSERT(topicp == nullptr);

    topicp = &topic;
}

inline
bool
BasePublisher::alloc_unsafe(
    Message*& msgp
)
{
    CORE_ASSERT(topicp != nullptr);

    msgp = topicp->alloc_unsafe();
    return msgp != nullptr;
}

inline
bool
BasePublisher::alloc(
    Message*& msgp
)
{
    core::os::SysLock::acquire();
    bool success = alloc_unsafe(msgp);
    core::os::SysLock::release();

    return success;
}

inline
bool
BasePublisher::publish_locally(
    Message& msg,
    bool     mustReschedule
)
{
    CORE_ASSERT(topicp != nullptr);

    return topicp->notify_locals(msg, core::os::Time::now(), mustReschedule);
}

inline
bool
BasePublisher::publish_remotely(
    Message& msg
)
{
    CORE_ASSERT(topicp != nullptr);

    return topicp->notify_remotes(msg, core::os::Time::now());
}

inline
bool
BasePublisher::has_topic(
    const BasePublisher& pub,
    const char*          namep
)
{
    return pub.topicp != nullptr && Topic::has_name(*pub.topicp, namep);
}

NAMESPACE_CORE_MW_END
