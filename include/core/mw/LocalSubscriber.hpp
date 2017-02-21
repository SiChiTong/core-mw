/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/BaseSubscriber.hpp>
#include <core/mw/StaticList.hpp>
#include <core/mw/MessagePtrQueue.hpp>
#include <functional>

NAMESPACE_CORE_MW_BEGIN

class Node;
class Message;


class LocalSubscriber:
    public BaseSubscriber
{
    friend class Node;
    friend class Topic;

public:
    using CallbackFunction = bool(const Message &msg, void* context);

private:
    Node* nodep;
    CallbackFunction* callback;
    MessagePtrQueue   msgp_queue;
    uint_least8_t     event_index;

    mutable StaticList<LocalSubscriber>::Link by_node;
    mutable StaticList<LocalSubscriber>::Link by_topic;

public:
    CallbackFunction*
    get_callback() const;

    void
    set_callback(
        CallbackFunction* callback
    );

    size_t
    get_queue_length() const;


public:
    bool
    fetch_unsafe(
        Message*&       msgp,
        core::os::Time& timestamp
    );

    bool
    notify_unsafe(
        Message&              msg,
        const core::os::Time& timestamp
    );

    bool
    fetch(
        Message*& msgp
    );

    bool
    fetch(
        Message*&       msgp,
        core::os::Time& timestamp
    );

    bool
    notify(
        Message&              msg,
        const core::os::Time& timestamp,
        bool                  mustReschedule = false
    );


protected:
    LocalSubscriber(
        Message*          queue_buf[],
        size_t            queue_length,
        CallbackFunction* callback = nullptr
    );
    virtual
    ~LocalSubscriber() = 0;
};


NAMESPACE_CORE_MW_END

#include <core/mw/Node.hpp>

NAMESPACE_CORE_MW_BEGIN


inline
LocalSubscriber::CallbackFunction*
LocalSubscriber::get_callback() const
{
    return callback;
}

inline
void
LocalSubscriber::set_callback(
    LocalSubscriber::CallbackFunction* callback
)
{
    this->callback = callback;
}

inline
size_t
LocalSubscriber::get_queue_length() const
{
    return msgp_queue.get_length();
}

inline
bool
LocalSubscriber::fetch_unsafe(
    Message*&       msgp,
    core::os::Time& timestamp
)
{
    if (msgp_queue.fetch_unsafe(msgp)) {
        timestamp = core::os::Time::now();
        return true;
    }

    return false;
}

inline
bool
LocalSubscriber::notify_unsafe(
    Message&              msg,
    const core::os::Time& timestamp
)
{
    (void)timestamp;

    if (nodep->get_enabled() && msgp_queue.post_unsafe(&msg)) {
        nodep->notify_unsafe(event_index);
        return true;
    }

    return false;
}

NAMESPACE_CORE_MW_END
