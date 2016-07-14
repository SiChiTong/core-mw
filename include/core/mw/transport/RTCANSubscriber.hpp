/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/common.hpp>
#include <core/mw/RemoteSubscriber.hpp>
#include <core/mw/TimestampedMsgPtrQueue.hpp>
#include <core/mw/StaticList.hpp>
#include <core/mw/BaseSubscriberQueue.hpp>
//#include <ch.h>

#include "rtcan.h"

NAMESPACE_CORE_MW_BEGIN

class Message;
class RTCANTransport;


class RTCANSubscriber:
   public RemoteSubscriber
{
   friend class RTCANTransport;

private:
   rtcan_id_t rtcan_id;

public:
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

   size_t
   get_queue_length() const;


public:
   RTCANSubscriber(
      RTCANTransport&               transport,
      TimestampedMsgPtrQueue::Entry queue_buf[],
      size_t                        queue_length
   );
   virtual
   ~RTCANSubscriber();
};

inline
bool
RTCANSubscriber::notify(
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
RTCANSubscriber::fetch(
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
