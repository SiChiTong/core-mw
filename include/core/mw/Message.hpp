/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <type_traits>

NAMESPACE_CORE_MW_BEGIN

class Transport;

/*! \brief Base class for all the messages
  */
class Message
{
public:
    using RefcountType = uint32_t;
    using Signature = uint32_t;
private:
 #if CORE_USE_BRIDGE_MODE
    Transport * sourcep CORE_PACKED;
#endif
    RefcountType refcount CORE_PACKED;

public:
    const uint8_t*
    get_raw_data() const;


#if CORE_USE_BRIDGE_MODE
    Transport*
    get_source() const;

    void
    set_source(
        Transport* sourcep
    );
#endif

    void
    acquire_unsafe();

    bool
    release_unsafe();

    void
    reset_unsafe();

    void
    acquire();

    bool
    release();

    void
    reset();


protected:
    Message();

public:
    static void
    copy(
        Message&       to,
        const Message& from,
        size_t         msg_size
    );

    static const Message&
    get_msg_from_raw_data(
        const uint8_t* datap
    );

    static constexpr size_t
    get_payload_size(
        size_t type_size
    );

    static size_t
    get_type_size(
        size_t payload_size
    );


    template <typename MessageType>
    static void
    copy(
        MessageType&       to,
        const MessageType& from
    );


    template <typename MessageType>
    static constexpr size_t
    get_payload_size();


    template <typename MessageType>
    static void
    reset_payload(
        MessageType& msg
    );
}

CORE_PACKED_ALIGNED;

/* ------------------------------------------------------------------------- */

inline
const uint8_t*
Message::get_raw_data() const
{
    return reinterpret_cast<const uint8_t*>(&refcount + 1);
}

// TODO: menate di stile
inline
const Message&
Message::get_msg_from_raw_data(
    const uint8_t* datap
)
{
    // Probably these casts are safe, as datap is taken from message::get_raw_data()
#if CORE_USE_BRIDGE_MODE
    return *reinterpret_cast<const Message*>(datap - (sizeof(Transport*) + sizeof(RefcountType)));

#else
    return *reinterpret_cast<const Message*>(datap - sizeof(RefcountType));
#endif
}

#if CORE_USE_BRIDGE_MODE

inline
Transport*
Message::get_source() const
{
    return sourcep;
}

inline
void
Message::set_source(
    Transport* sourcep
)
{
    this->sourcep = sourcep;
}
#endif // CORE_USE_BRIDGE_MODE


inline
void
Message::acquire_unsafe()
{
    // CORE_ASSERT(refcount < ((1 << (8 * sizeof(refcount) - 1)) - 1));
    // CORE_ASSERT(refcount < (std::numeric_limits<RefcountType>::max() - 1));

    ++refcount;
}

inline
bool
Message::release_unsafe()
{
    CORE_ASSERT(refcount > 0);

    return --refcount > 0;
}

inline
void
Message::reset_unsafe()
{
#if CORE_USE_BRIDGE_MODE
    sourcep = nullptr;
#endif
    refcount = 0;
}

inline
Message::Message()
    :
#if CORE_USE_BRIDGE_MODE
    sourcep(nullptr),
#endif
    refcount(0)
{}


inline constexpr
size_t
Message::get_payload_size(
    size_t type_size
)
{
#if CORE_USE_BRIDGE_MODE
    return type_size - (sizeof(Transport*) + sizeof(RefcountType));

#else
    return type_size - sizeof(RefcountType);
#endif
}

inline
size_t
Message::get_type_size(
    size_t payload_size
)
{
#if CORE_USE_BRIDGE_MODE
    return payload_size + (sizeof(Transport*) + sizeof(RefcountType));

#else
    return payload_size + sizeof(RefcountType);
#endif
}

template <typename MessageType>
inline
void
Message::copy(
    MessageType&       to,
    const MessageType& from
)
{
    static_assert(std::is_base_of<Message, MessageType>::value, "MessageType does not inherit from Message");
    return copy(to, from, sizeof(MessageType));
}

template <typename MessageType>
inline constexpr
size_t
Message::get_payload_size()
{
    static_assert(std::is_base_of<Message, MessageType>::value, "MessageType does not inherit from Message");
    return get_payload_size(sizeof(MessageType));
}

template <typename MessageType>
inline
void
Message::reset_payload(
    MessageType& msg
)
{
    static_assert(std::is_base_of<Message, MessageType>::value, "MessageType does not inherit from Message");
    memset(&msg.refcount + 1, 0, get_payload_size<MessageType>());
}

NAMESPACE_CORE_MW_END
