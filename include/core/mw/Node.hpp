/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/NamingTraits.hpp>
#include <core/mw/StaticList.hpp>
#include <core/os/SpinEvent.hpp>
#include <core/os/Time.hpp>
#include <core/mw/MgmtMsg.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;
class Topic;
class LocalPublisher;
class LocalSubscriber;
template <typename MessageType>
class Publisher;
template <typename MessageType>
class SubscriberExtBuf;
template <typename MessageType, unsigned QUEUE_LENGTH>
class Subscriber;


class Node:
   private Uncopyable
{
   friend class Middleware;

private:
   const char* const           namep;
   StaticList<LocalPublisher>  publishers;
   StaticList<LocalSubscriber> subscribers;
   core::os::SpinEvent         event;
   core::os::Time timeout;

   mutable StaticList<Node>::Link by_middleware;

public:
   const char*
   get_name() const;


   const StaticList<LocalPublisher>&
   get_publishers() const;


   const StaticList<LocalSubscriber>&
   get_subscribers() const;

   bool
   get_enabled() const;

   void
   set_enabled(
      bool enabled
   );


   template <typename MT>
   bool
   advertise(
      Publisher<MT>&        pub,
      const char*           namep,
      const core::os::Time& publish_timeout = core::os::Time::INFINITE
   );


   template <typename MT>
   bool
   subscribe(
      SubscriberExtBuf<MT>& sub,
      const char*           namep,
      MT                    mgpool_buf[]
   );


   template <typename MT, unsigned QL>
   bool
   subscribe(
      Subscriber<MT, QL>& sub,
      const char* namep
   );

   void
   notify_unsafe(
      unsigned event_index,
      bool     mustReschedule = false
   );

   void
   notify_stop_unsafe();

   void
   notify(
      unsigned event_index
   );

   void
   notify_stop();

   bool
   spin(
      const core::os::Time& timeout = core::os::Time::INFINITE
   );


private:
   bool
   advertise(
      LocalPublisher&       pub,
      const char*           namep,
      const core::os::Time& publish_timeout,
      size_t                msg_size
   );

   bool
   subscribe(
      LocalSubscriber& sub,
      const char*      namep,
      Message          msgpool_buf[],
      size_t           msg_size
   );


public:
   Node(
      const char* namep,
      bool        enabled = true
   );
   ~Node();

public:
   static bool
   has_name(
      const Node& node,
      const char* namep
   );
};


NAMESPACE_CORE_MW_END

#include <core/os/Thread.hpp>
#include <core/mw/Subscriber.hpp>

NAMESPACE_CORE_MW_BEGIN


inline
const char*
Node::get_name() const
{
   return namep;
}

inline
const StaticList<LocalPublisher>&
Node::get_publishers() const
{
   return publishers;
}

inline
const StaticList<LocalSubscriber>&
Node::get_subscribers() const
{
   return subscribers;
}

inline
bool
Node::get_enabled() const
{
   return event.get_thread() != NULL;
}

inline
void
Node::set_enabled(
   bool enabled
)
{
   event.set_thread(enabled ? &core::os::Thread::self() : NULL);
}

template <typename MessageType>
inline
bool
Node::advertise(
   Publisher<MessageType>& pub,
   const char*             namep,
   const core::os::Time&   publish_timeout
)
{
   return advertise(pub, namep, publish_timeout, sizeof(MessageType));
}

template <typename MT>
inline
bool
Node::subscribe(
   SubscriberExtBuf<MT>& sub,
   const char*           namep,
   MT                    msgpool_buf[]
)
{
   return subscribe(sub, namep, msgpool_buf, sizeof(MT));
}

template <typename MT, unsigned QL>
bool
Node::subscribe(
   Subscriber<MT, QL>& sub,
   const char* namep
)
{
   return subscribe(sub, namep, sub.msgpool_buf, sizeof(MT));
}

inline
void
Node::notify_unsafe(
   unsigned event_index,
   bool     mustReschedule
)
{
   event.signal_unsafe(event_index, mustReschedule);
}

inline
void
Node::notify_stop_unsafe()
{
   // Just signal a dummy unlikely event
   event.signal_unsafe(core::os::SpinEvent::MAX_INDEX);
}

inline
void
Node::notify(
   unsigned event_index
)
{
   event.signal(event_index);
}

inline
void
Node::notify_stop()
{
   // Just signal a dummy unlikely event
   event.signal(core::os::SpinEvent::MAX_INDEX);
}

inline
bool
Node::has_name(
   const Node& node,
   const char* namep
)
{
   return namep != NULL && 0 == strncmp(node.get_name(), namep, NamingTraits<Node>::MAX_LENGTH);
}

NAMESPACE_CORE_MW_END
