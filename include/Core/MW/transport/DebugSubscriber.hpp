/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/RemoteSubscriber.hpp>
#include <Core/MW/TimestampedMsgPtrQueue.hpp>
#include <Core/MW/StaticList.hpp>
#include <Core/MW/BaseSubscriberQueue.hpp>
//#include <ch.h>

NAMESPACE_CORE_MW_BEGIN

class Message;
class DebugTransport;


class DebugSubscriber:
   public RemoteSubscriber
{
   friend class DebugTransport;

private:
   TimestampedMsgPtrQueue tmsgp_queue;

private:
   mutable BaseSubscriberQueue::Link by_transport_notify;

public:
   size_t
   get_queue_length() const;

   size_t
   get_queue_count() const;

   bool
   fetch_unsafe(
      Message*& msgp,
      Time&     timestamp
   );

   bool
   notify_unsafe(
      Message&    msg,
      const Time& timestamp
   );

   bool
   fetch(
      Message*& msgp,
      Time&     timestamp
   );

   bool
   notify(
      Message&    msg,
      const Time& timestamp,
      bool        mustReschedule = false
   );


public:
   DebugSubscriber(
      DebugTransport&               transport,
      TimestampedMsgPtrQueue::Entry queue_buf[],
      size_t                        queue_length
   );
   ~DebugSubscriber();
};


inline
bool
DebugSubscriber::notify(
   Message&    msg,
   const Time& timestamp,
   bool        mustReschedule
)
{
   SysLock::acquire();
   bool success = notify_unsafe(msg, timestamp);
   SysLock::release();

   return success;
}

inline
bool
DebugSubscriber::fetch(
   Message*& msgp,
   Time&     timestamp
)
{
   SysLock::acquire();
   bool success = fetch_unsafe(msgp, timestamp);
   SysLock::release();

   return success;
}

NAMESPACE_CORE_MW_END
