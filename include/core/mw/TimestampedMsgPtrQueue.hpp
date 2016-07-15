/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/ArrayQueue.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;


class TimestampedMsgPtrQueue
{
public:
   struct Entry {
      Message* msgp;
      ::core::os::Time     timestamp;

      Entry&
      operator=(
         const Entry& other
      );


      Entry();
      Entry(
         Message*    msgp,
         const ::core::os::Time& timestamp
      );
   };

private:
   ArrayQueue<Entry> impl;

public:
   size_t
   get_count_unsafe() const;

   bool
   post_unsafe(
      Entry& entry
   );

   bool
   fetch_unsafe(
      Entry& entry
   );

   size_t
   get_length() const;

   size_t
   get_count() const;

   bool
   post(
      Entry& entry
   );

   bool
   fetch(
      Entry& entry
   );


public:
   TimestampedMsgPtrQueue(
      Entry  array[],
      size_t length
   );
};


inline
TimestampedMsgPtrQueue::Entry&
TimestampedMsgPtrQueue::Entry::operator=(
   const Entry& other
)
{
   msgp      = other.msgp;
   timestamp = other.timestamp;
   return *this;
}

inline
TimestampedMsgPtrQueue::Entry::Entry(
   Message*    msgp,
   const ::core::os::Time& timestamp
)
   :
   msgp(msgp),
   timestamp(timestamp)
{
   CORE_ASSERT(msgp != NULL);
}

inline
bool
TimestampedMsgPtrQueue::post_unsafe(
   Entry& entry
)
{
   return impl.post_unsafe(entry);
}

inline
bool
TimestampedMsgPtrQueue::fetch_unsafe(
   Entry& entry
)
{
   return impl.fetch_unsafe(entry);
}

inline
size_t
TimestampedMsgPtrQueue::get_length() const
{
   return impl.get_length();
}

inline
size_t
TimestampedMsgPtrQueue::get_count() const
{
   return impl.get_count();
}

inline
bool
TimestampedMsgPtrQueue::post(
   Entry& entry
)
{
   return impl.post(entry);
}

inline
bool
TimestampedMsgPtrQueue::fetch(
   Entry& entry
)
{
   return impl.fetch(entry);
}

inline
TimestampedMsgPtrQueue::Entry::Entry()
   :
   msgp()
{}


NAMESPACE_CORE_MW_END
