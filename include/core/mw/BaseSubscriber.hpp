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

/*! \brief Subscriber base class
 */
class BaseSubscriber:
    private core::Uncopyable
{
public:
    Topic*
    get_topic() const;

    /*! \brief Get the length of the queue
     */
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

    /*! \brief Fetch a message
     *
     * \return success
     * \retval true A message has been fetched
     * \retval false There were no message pending
     */
    virtual bool
    fetch(
        Message*&       msgp,  //!< [in/out] the fetched message
        core::os::Time& timestamp //!< [out] message timestamp
    ) = 0;

    /*! \brief Release a previously fetched message
     *
     * \pre The message must have been fetched
     *
     * \return success
     */
    bool
    release(
        Message& msg //!< [in] message to be released
    );

    static bool
    has_topic(
        const BaseSubscriber& sub,
        const char*           namep
    );

protected:
    BaseSubscriber();
    virtual
    ~BaseSubscriber() = 0;

private:
    Topic* topicp;
};

/* ------------------------------------------------------------------------- */

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
    CORE_ASSERT(topicp == nullptr);

    topicp = &topic;
}

NAMESPACE_CORE_MW_END
