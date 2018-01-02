/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
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

/*! \brief A subscriber
 *
 * It is possible to specify a callback function that will be called by Node::spin if a message has been received.
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
    /*! \brief Type of the Message
     */
    using MessageType = MESSAGE_TYPE;

    /*! \brief Type of the callback function
     *
     * \param msg received message
     * \param context context (such as pointer to the node the subscriber belongs to)
     *
     * \return success
     */
    using CallbackFunction = typename SubscriberExtBuf<MessageType>::CallbackFunction;

public:
    /*! \brief Constructor
     */
    Subscriber(
        CallbackFunction* callback = nullptr //!< [in] callback function on data reception.
    );
    ~Subscriber();

private:
    MessageType  msgpool_buf[QUEUE_LENGTH];
    MessageType* queue_buf[QUEUE_LENGTH];
};

/* ------------------------------------------------------------------------- */

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
