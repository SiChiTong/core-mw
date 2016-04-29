/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>

NAMESPACE_CORE_MW_BEGIN


#if !defined(CORE_MESSAGE_REFCOUNT_TYPE) || defined(__DOXYGEN__)
#define CORE_MESSAGE_REFCOUNT_TYPE   uint16_t
#endif

#if !defined(CORE_MESSAGE_LENGTH_TYPE) || defined(__DOXYGEN__)
#define CORE_MESSAGE_LENGTH_TYPE     uint8_t
#endif

class Transport;


// TODO: Add refcount as a decorator, user should only declare the contents type
class Message
{
public:
	typedef CORE_MESSAGE_REFCOUNT_TYPE RefcountType;
	typedef CORE_MESSAGE_LENGTH_TYPE   LengthType;

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

	static size_t
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
	static size_t
	get_payload_size();


	template <typename MessageType>
	static void
	reset_payload(
			MessageType& msg
	);
}
CORE_PACKED;


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
#if CORE_USE_BRIDGE_MODE
	return *reinterpret_cast<const Message*>(
	   datap - (sizeof(Transport*) + sizeof(RefcountType))
	);

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
	CORE_ASSERT(refcount < ((1 << (8 * sizeof(refcount) - 1)) - 1));

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
	sourcep = NULL;
#endif
	refcount = 0;
}

inline
Message::Message()
	:
#if CORE_USE_BRIDGE_MODE
	sourcep(NULL),
#endif
	refcount(0)
{}


inline
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
	static_cast_check<MessageType, Message>();
	return copy(to, from, sizeof(MessageType));
}

template <typename MessageType>
inline
size_t
Message::get_payload_size()
{
	static_cast_check<MessageType, Message>();
	return get_payload_size(sizeof(MessageType));
}

template <typename MessageType>
inline
void
Message::reset_payload(
		MessageType& msg
)
{
	static_cast_check<MessageType, Message>();
	memset(&msg.refcount + 1, 0, get_payload_size<MessageType>());
}

NAMESPACE_CORE_MW_END
