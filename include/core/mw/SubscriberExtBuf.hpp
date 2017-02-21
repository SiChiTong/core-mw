/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once
#include <type_traits>

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/LocalSubscriber.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;

/*! \brief TODO
 *
 * \tparam MESSAGE_TYPE type of the message to be published
 */
template <typename MESSAGE_TYPE>
class SubscriberExtBuf:
    public LocalSubscriber
{
public:
    /*! \brief Type of the Message
     *
   */
    using MessageType = MESSAGE_TYPE;

    /*! \brief Type of the callback function
     *
     * \param msg received message
     * \param context context (such as pointer to the node the subscriber belongs to)
     *
     * \return success
     */
    using CallbackFunction = bool(const MessageType &msg, void* context);

public:
    /*! \brief Get the callback to be invoked on message reception
     *
     */
    CallbackFunction*
    get_callback() const;


    /*! \brief Set the callback to be invoked on message reception
     *
     */
    void
    set_callback(
        CallbackFunction* callback //!< [in] pointer to callback function
    );


    /*! \brief Fetch a message from the queue
     *
     * \return success
     * \retval true A message has been fetched
     * \retval false There were no message pending in the queue
     */
    bool
    fetch(
        MessageType*& msgp //!< will reference the fetched message
    );


    /*! \brief Fetch a message from the queue
     *
     * \return success
     * \retval true A message has been fetched
     * \retval false There were no message pending in the queue
     */
    bool
    fetch(
        MessageType*&   msgp, //!< will reference the fetched message
        core::os::Time& timestamp //!< message timestamp
    );


    /*! \brief Releases the message
     *
     */
    bool
    release(
        MessageType& msg //!< message to be released
    );


public:
    SubscriberExtBuf(
        MessageType*      queue_buf[],
        size_t            queue_length,
        CallbackFunction* callback = nullptr
    );
    ~SubscriberExtBuf();
};


template <typename MT>
inline
bool
SubscriberExtBuf<MT>::fetch(
    MT*& msgp
)
{
    return LocalSubscriber::fetch(
        reinterpret_cast<Message*&>(msgp));
}

template <typename MT>
inline
bool
SubscriberExtBuf<MT>::fetch(
    MT*&            msgp,
    core::os::Time& timestamp
)
{
    static_cast_check<MT, Message>();
    return LocalSubscriber::fetch(
        reinterpret_cast<Message*&>(msgp), timestamp
    );
}

template <typename MT>
inline
typename SubscriberExtBuf<MT>::CallbackFunction
* SubscriberExtBuf<MT>::get_callback() const
{
    return reinterpret_cast<CallbackFunction*>(LocalSubscriber::get_callback());
}

template <typename MT>
inline
void
SubscriberExtBuf<MT>::set_callback(
    SubscriberExtBuf<MT>::CallbackFunction* callback
)
{
    LocalSubscriber::set_callback(reinterpret_cast<LocalSubscriber::CallbackFunction*>(callback));
}

template <typename MT>
inline
bool
SubscriberExtBuf<MT>::release(
    MT& msg
)
{
    static_cast_check<MT, Message>();
    return LocalSubscriber::release(static_cast<Message&>(msg));
}

template <typename MT>
inline
SubscriberExtBuf<MT>::SubscriberExtBuf(
    MT*               queue_buf[],
    size_t            queue_length,
    CallbackFunction* callback
)
    :
    LocalSubscriber(reinterpret_cast<Message**>(queue_buf), queue_length,
                    reinterpret_cast<LocalSubscriber::CallbackFunction*>(callback))
{
    static_cast_check<MT, Message>();
}

template <typename MT>
inline
SubscriberExtBuf<MT>::~SubscriberExtBuf()
{
    static_cast_check<MT, Message>();
}

NAMESPACE_CORE_MW_END
