/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Topic.hpp>
#include <core/mw/Middleware.hpp>
#include <core/mw/NamingTraits.hpp>
#include <core/mw/Message.hpp>
#include <core/mw/Transport.hpp>

NAMESPACE_CORE_MW_BEGIN

bool
Topic::notify_locals_unsafe(
   Message&    msg,
   const core::os::Time& timestamp
)
{
   if (has_local_subscribers()) {
      for (StaticList<LocalSubscriber>::IteratorUnsafe i = local_subscribers.begin_unsafe(); i != local_subscribers.end_unsafe(); ++i) {
         msg.acquire_unsafe();

         if (!i->notify_unsafe(msg, timestamp)) {
            msg.release_unsafe();
         }
      }
   }

   return true;
}

bool
Topic::notify_remotes_unsafe(
   Message&    msg,
   const core::os::Time& timestamp
)
{
   if (has_remote_subscribers()) {
      for (StaticList<RemoteSubscriber>::IteratorUnsafe i = remote_subscribers.begin_unsafe(); i != remote_subscribers.end_unsafe(); ++i) {
#if CORE_USE_BRIDGE_MODE
         CORE_ASSERT(i->get_transport() != NULL);

         if (msg.get_source() == i->get_transport()) {
            continue; // Do not send back to source transport
         }
#endif

         msg.acquire_unsafe();

         if (!i->notify_unsafe(msg, timestamp)) {
            msg.release_unsafe();
         }
      }
   }

   return true;
} // Topic::notify_remotes_unsafe

bool
Topic::forward_copy_unsafe(
   const Message& msg,
   const core::os::Time&    timestamp
)
{
   bool all = true;

   for (StaticList<RemoteSubscriber>::IteratorUnsafe i = remote_subscribers.begin_unsafe(); i != remote_subscribers.end_unsafe(); ++i) {
#if CORE_USE_BRIDGE_MODE
      CORE_ASSERT(i->get_transport() != NULL);

      if (msg.get_source() == i->get_transport()) {
         continue; // Do not send back to source transport
      }
#endif

      Message* msgp;

      if (alloc_unsafe(msgp)) {
         Message::copy(*msgp, msg, get_type_size());
         patch_pubsub_msg(*msgp, *i->get_transport());
         msgp->acquire_unsafe();

         if (!i->notify_unsafe(*msgp, timestamp)) {
            free_unsafe(*msgp);
            all = false;
         }
      } else {
         all = false;
      }
   }

   return all;
} // Topic::forward_copy_unsafe

Message*
Topic::alloc()
{
	core::os::SysLock::acquire();
   Message* msgp = alloc_unsafe();
   core::os::SysLock::release();

   return msgp;
}

bool
Topic::notify_locals(
   Message&    msg,
   const core::os::Time& timestamp,
   bool        mustReschedule
)
{
   if (has_local_subscribers()) {
      for (StaticList<LocalSubscriber>::Iterator i = local_subscribers.begin(); i != local_subscribers.end(); ++i) {
         msg.acquire();

         if (!i->notify(msg, timestamp, mustReschedule)) {
            msg.release();
         }
      }
   }

   return true;
}

bool
Topic::notify_remotes(
   Message&    msg,
   const core::os::Time& timestamp
)
{
   {
	   core::os::SysLock::Scope lock;

      if (!has_remote_subscribers()) {
         return true;
      }
   }

   for (StaticList<RemoteSubscriber>::Iterator i = remote_subscribers.begin(); i != remote_subscribers.end(); ++i) {
#if CORE_USE_BRIDGE_MODE
      {
    	  core::os::SysLock::Scope lock;
         CORE_ASSERT(i->get_transport() != NULL);

         if (msg.get_source() == i->get_transport()) {
            continue; // Do not send back to source transport
         }
      }
#endif

      msg.acquire();

      if (!i->notify(msg, timestamp)) {
         msg.release();
      }
   }

   return true;
} // Topic::notify_remotes

bool
Topic::forward_copy(
   const Message& msg,
   const core::os::Time&    timestamp
)
{
   bool all = true;

   for (StaticList<RemoteSubscriber>::Iterator i = remote_subscribers.begin(); i != remote_subscribers.end(); ++i) {
#if CORE_USE_BRIDGE_MODE
      CORE_ASSERT(i->get_transport() != NULL);

      if (msg.get_source() == i->get_transport()) {
         continue; // Do not send back to source transport
      }
#endif

      Message* msgp;

      if (alloc(msgp)) {
         Message::copy(*msgp, msg, get_type_size());
         patch_pubsub_msg(*msgp, *i->get_transport());
         msgp->acquire();

         if (!i->notify(*msgp, timestamp)) {
            free(*msgp);
            all = false;
         }
      } else {
         all = false;
      }
   }

   return all;
} // Topic::forward_copy

void
Topic::advertise(
   LocalPublisher& pub,
   const core::os::Time&     publish_timeout
)
{
   (void)pub;
   core::os::SysLock::acquire();

   if (this->publish_timeout > publish_timeout) {
      this->publish_timeout = publish_timeout;
   }

   ++num_local_publishers;
   core::os::SysLock::release();
}

void
Topic::advertise(
   RemotePublisher& pub,
   const core::os::Time&      publish_timeout
)
{
   (void)pub;
   core::os::SysLock::acquire();

   if (this->publish_timeout > publish_timeout) {
      this->publish_timeout = publish_timeout;
   }

   ++num_remote_publishers;
   core::os::SysLock::release();
}

void
Topic::subscribe(
   LocalSubscriber& sub,
   size_t           queue_length
)
{
   core::os::SysLock::acquire();

   if (max_queue_length < queue_length) {
      max_queue_length = queue_length;
   }

   local_subscribers.link_unsafe(sub.by_topic);
   core::os::SysLock::release();
}

void
Topic::subscribe(
   RemoteSubscriber& sub,
   size_t            queue_length
)
{
	core::os::SysLock::acquire();

   if (max_queue_length < queue_length) {
      max_queue_length = queue_length;
   }

   remote_subscribers.link_unsafe(sub.by_topic);
   core::os::SysLock::release();
}

void
Topic::patch_pubsub_msg(
   Message&   msg,
   Transport& transport
) const
{
   if (this != &Middleware::instance.get_mgmt_topic()) {
      return;
   }

   MgmtMsg& mgmt_msg = static_cast<MgmtMsg&>(msg);

   switch (mgmt_msg.type) {
     case MgmtMsg::ADVERTISE:
     case MgmtMsg::SUBSCRIBE_REQUEST:
     case MgmtMsg::SUBSCRIBE_RESPONSE:
     {
        // TODO: Get the topic from a reference in function parameters, to speed up
        Topic* topicp = Middleware::instance.find_topic(mgmt_msg.pubsub.topic);

        if (topicp != NULL) {
           transport.fill_raw_params(*topicp, mgmt_msg.pubsub.raw_params);
        }

        break;
     }
   }
} // Topic::patch_pubsub_msg

Topic::Topic(
   const char* namep,
   size_t      type_size,
   bool        forwarding
)
   :
   namep(namep),
   publish_timeout(core::os::Time::INFINITE),
   msg_pool(type_size),
   num_local_publishers(0),
   num_remote_publishers(0),
   max_queue_length(0),
#if CORE_USE_BRIDGE_MODE
   forwarding(CORE_DEFAULT_FORWARDING_RULE),
#endif
   by_middleware(*this)
{
   CORE_ASSERT(is_identifier(namep, NamingTraits<Topic>::MAX_LENGTH));

   (void)forwarding;
}

NAMESPACE_CORE_MW_END
