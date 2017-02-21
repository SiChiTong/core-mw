/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/SubscriberExtBuf.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;
class Node;

/*! \brief A publisher
 *
 * \tparam MESSAGE_TYPE type of the message to be published
 * \tparam QUEUE_LENGTH length of the message queue
 *
 * \note MESSAGE_TYPE must refer to a class inherited from core::mw::Message
 */
template <typename MESSAGE_TYPE, unsigned QUEUE_LENGTH>
class Subscriber:
    public SubscriberExtBuf<MESSAGE_TYPE>
{
    friend class Node;

public:
    using MessageType      = MESSAGE_TYPE;
    using CallbackFunction = typename SubscriberExtBuf<MessageType>::CallbackFunction;

private:
    MessageType  msgpool_buf[QUEUE_LENGTH];
    MessageType* queue_buf[QUEUE_LENGTH];

public:
    Subscriber(
        CallbackFunction* callback = nullptr
    );
    ~Subscriber();
};


template <typename MT, unsigned QL>
inline
Subscriber<MT, QL>::Subscriber(
    CallbackFunction* callback
)
    :
    SubscriberExtBuf<MT>(queue_buf, QL, callback)
{}


template <typename MT, unsigned QL>
inline
Subscriber<MT, QL>::~Subscriber() {}


NAMESPACE_CORE_MW_END
