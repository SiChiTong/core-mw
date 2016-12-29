/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/Middleware.hpp>
#include <core/mw/NamingTraits.hpp>
#include <core/os/Thread.hpp>
#include <core/mw/MgmtMsg.hpp>
#include <core/mw/BootMsg.hpp>
#include <core/mw/Topic.hpp>
#include <core/mw/Node.hpp>
#include <core/mw/Transport.hpp>
#include <core/mw/Publisher.hpp>
#include <core/mw/Subscriber.hpp>
#include <core/os/ScopedLock.hpp>

NAMESPACE_CORE_MW_BEGIN

void
Middleware::initialize(
   void*                      mgmt_stackp,
   size_t                     mgmt_stacklen,
   core::os::Thread::Priority mgmt_priority,
   void*                      boot_stackp,
   size_t                     boot_stacklen,
   core::os::Thread::Priority boot_priority
)
{
   CORE_ASSERT(mgmt_stackp != NULL);
   CORE_ASSERT(mgmt_stacklen > 0);

   lists_lock.initialize();

   this->mgmt_stackp   = mgmt_stackp;
   this->mgmt_stacklen = mgmt_stacklen;
   this->mgmt_priority = mgmt_priority;

   topics.link(mgmt_topic.by_middleware);
#if CORE_IS_BOOTLOADER_BRIDGE
   topics.link(boot_topic.by_middleware);
   topics.link(bootmaster_topic.by_middleware);
#endif
} // Middleware::initialize

void
Middleware::start()
{
   mgmt_threadp = core::os::Thread::create_static(mgmt_stackp, mgmt_stacklen, mgmt_priority, mgmt_threadf, NULL, "CORE_MGMT");
   CORE_ASSERT(mgmt_threadp != NULL);

   // Wait until the info topic is fully initialized
   core::os::SysLock::acquire();

   while (!mgmt_topic.has_local_publishers() || !mgmt_topic.has_local_subscribers()) {
      core::os::SysLock::release();
      core::os::Thread::sleep(core::os::Time::ms(100));
      core::os::SysLock::acquire();
   }

   core::os::SysLock::release();
} // Middleware::start

void
Middleware::stop()
{
   {
      core::os::SysLock::Scope lock;

      if (!stopped) {
         stopped = true;
      } else {
         return;
      }
   }

   // Stop all nodes
   for (;;) {
      for (StaticList<Node>::Iterator i = nodes.begin(); i != nodes.end(); ++i) {
         i->notify_stop();
      }

      core::os::Thread::sleep(core::os::Time::ms(500)); // TODO: Configure delay
      {
         core::os::SysLock::Scope lock;

         if (num_running_nodes == 0) {
            break;
         }
      }
   }
} // Middleware::stop

bool
Middleware::stop_remote(
   const char* namep
)
{
   MgmtMsg* msgp;

   if (mgmt_pub.alloc(msgp)) {
      msgp->type = MgmtMsg::STOP;
      strncpy(msgp->module.name, namep, NamingTraits<Middleware>::MAX_LENGTH);
      return mgmt_pub.publish_remotely(*msgp);
   }

   return false;
}

bool
Middleware::reboot_remote(
   const char* namep,
   bool        bootload
)
{
   MgmtMsg* msgp;

   if (mgmt_pub.alloc(msgp)) {
      msgp->type = bootload ? MgmtMsg::BOOTLOAD : MgmtMsg::REBOOT;
      strncpy(msgp->module.name, namep, NamingTraits<Middleware>::MAX_LENGTH);
      return mgmt_pub.publish_remotely(*msgp);
   }

   return false;
}

void
Middleware::add(
   Node& node
)
{
   core::os::SysLock::acquire();

   nodes.link_unsafe(node.by_middleware);
   ++num_running_nodes;
   core::os::SysLock::release();
}

void
Middleware::add(
   Transport& transport
)
{
   transports.link(transport.by_middleware);
}

void
Middleware::add(
   Topic& topic
)
{
   CORE_ASSERT(find_topic(topic.get_name()) == NULL);

   topics.link(topic.by_middleware);
}

bool
Middleware::advertise(
   LocalPublisher&       pub,
   const char*           namep,
   const core::os::Time& publish_timeout,
   size_t                type_size
)
{
   lists_lock.acquire();
   Topic* topicp = touch_topic(namep, type_size);

   if (topicp == NULL) {
      return false;
   }

   pub.notify_advertised(*topicp);
   topicp->advertise(pub, publish_timeout);
   lists_lock.release();

   if (topicp != &mgmt_topic) {
#if CORE_IS_BOOTLOADER_BRIDGE
      if (!(Topic::has_name(*topicp, BOOTLOADER_TOPIC_NAME) || Topic::has_name(*topicp, BOOTLOADER_MASTER_TOPIC_NAME))) {
#endif
      {
         core::os::SysLock::Scope lock;

         if (mgmt_pub.get_topic() == NULL) {
            return true;
         }
      }

      MgmtMsg* msgp;

      if (mgmt_pub.alloc(msgp)) {
         msgp->type = MgmtMsg::ADVERTISE;
         strncpy(msgp->pubsub.topic, topicp->get_name(), NamingTraits<Topic>::MAX_LENGTH);
         msgp->pubsub.payload_size = static_cast<uint16_t>(topicp->get_payload_size());
         mgmt_pub.publish_remotely(*msgp);
      }
   }

#if CORE_IS_BOOTLOADER_BRIDGE
} // Middleware::advertise
#endif

   return true;
} // Middleware::advertise

bool
Middleware::advertise(
   RemotePublisher&      pub,
   const char*           namep,
   const core::os::Time& publish_timeout,
   size_t                type_size
)
{
   lists_lock.acquire();
   Topic* topicp = touch_topic(namep, type_size);

   if (topicp == NULL) {
      return false;
   }

   pub.notify_advertised(*topicp);
   topicp->advertise(pub, publish_timeout);
   lists_lock.release();

   return true;
}

bool
Middleware::subscribe(
   LocalSubscriber& sub,
   const char*      namep,
   Message          msgpool_buf[],
   size_t           msgpool_buflen,
   size_t           type_size
)
{
   lists_lock.acquire();
   Topic* topicp = touch_topic(namep, type_size);

   if (topicp == NULL) {
      return false;
   }

   topicp->extend_pool(msgpool_buf, msgpool_buflen);
   sub.notify_subscribed(*topicp);
   topicp->subscribe(sub, msgpool_buflen);
   lists_lock.release();

   if (topicp != &mgmt_topic) {
#if CORE_IS_BOOTLOADER_BRIDGE
      if (!(Topic::has_name(*topicp, BOOTLOADER_TOPIC_NAME) || Topic::has_name(*topicp, BOOTLOADER_MASTER_TOPIC_NAME))) {
#endif

      for (StaticList<Transport>::Iterator i = transports.begin(); i != transports.end(); ++i) {
         {
            core::os::SysLock::Scope lock;

            if (mgmt_pub.get_topic() == NULL) {
               return true;
            }
         }

         MgmtMsg* msgp;

         if (mgmt_pub.alloc(msgp)) {
            msgp->type = MgmtMsg::SUBSCRIBE_REQUEST;
            strncpy(msgp->pubsub.topic, topicp->get_name(), NamingTraits<Topic>::MAX_LENGTH);
            msgp->pubsub.payload_size = static_cast<uint16_t>(topicp->get_payload_size());
            msgp->pubsub.queue_length = static_cast<uint16_t>(msgpool_buflen);
            mgmt_pub.publish_remotely(*msgp);
         }
      }
   }

#if CORE_IS_BOOTLOADER_BRIDGE
} // Middleware::subscribe
#endif

   return true;
} // Middleware::subscribe

bool
Middleware::subscribe(
   RemoteSubscriber& sub,
   const char*       namep,
   Message           msgpool_buf[],
   size_t            msgpool_buflen,
   size_t            type_size
)
{
   (void)type_size; // FIXME

   lists_lock.acquire();
   Topic* topicp = find_topic(namep);
   CORE_ASSERT(topicp != NULL);
   topicp->extend_pool(msgpool_buf, msgpool_buflen);
   sub.notify_subscribed(*topicp);
   topicp->subscribe(sub, msgpool_buflen);
   lists_lock.release();

   return true;
}

void
Middleware::confirm_stop(
   const Node& node
)
{
   core::os::SysLock::acquire();

   CORE_ASSERT(num_running_nodes > 0);
   CORE_ASSERT(nodes.contains_unsafe(node));
   --num_running_nodes;
   nodes.unlink_unsafe(node.by_middleware);
   core::os::SysLock::release();
}

Topic*
Middleware::find_topic(
   const char* namep
)
{
   lists_lock.acquire();
   Topic* topicp = topics.find_first(Topic::has_name, namep);
   lists_lock.release();
   return topicp;
}

Node*
Middleware::find_node(
   const char* namep
)
{
   lists_lock.acquire();
   Node* nodep = nodes.find_first(Node::has_name, namep);
   lists_lock.release();
   return nodep;
}

Topic*
Middleware::touch_topic(
   const char* namep,
   size_t      type_size
)
{
   lists_lock.acquire();

   // Check if there is a topic with the desired name
   Topic* topicp = find_topic(namep);

   if (topicp == NULL) {
      // Allocate a new topic
      topicp = new Topic(namep, type_size);

      if (topicp != NULL) {
         topics.link(topicp->by_middleware);
      }
   }

   lists_lock.release();
   return topicp;
} // Middleware::touch_topic

void
Middleware::do_mgmt_thread()
{
   core::os::SysLock::acquire();

   mgmt_node.set_enabled(true);
   core::os::SysLock::release();
   mgmt_node.advertise(mgmt_pub, mgmt_topic.get_name(), core::os::Time::INFINITE);
   mgmt_node.subscribe(mgmt_sub, mgmt_topic.get_name(), mgmt_msg_buf);

   // Tell it is alive
   MgmtMsg* msgp;

   if (mgmt_pub.alloc(msgp)) {
      Message::reset_payload(*msgp);
      msgp->type = MgmtMsg::ALIVE;
      strncpy(msgp->module.name, module_namep, NamingTraits<Middleware>::MAX_LENGTH);
      core::os::SysLock::acquire();
      msgp->module.flags.stopped = is_stopped() ? 1 : 0;
      core::os::SysLock::release();
      mgmt_pub.publish_remotely(*msgp);
   }

   // Message dispatcher
   for (;;) {
      if (mgmt_node.spin(core::os::Time::ms(MGMT_TIMEOUT_MS))) {
         core::os::Time deadline;

         while (mgmt_sub.fetch(msgp, deadline)) {
            switch (msgp->type) {
              case MgmtMsg::ADVERTISE:
              {
                 do_cmd_advertise(*msgp);
#if CORE_USE_BRIDGE_MODE
                 mgmt_topic.forward_copy(*msgp, deadline);
#endif // CORE_USE_BRIDGE_MODE
                 mgmt_sub.release(*msgp);
                 break;
              }
              case MgmtMsg::SUBSCRIBE_REQUEST:
              {
                 do_cmd_subscribe_request(*msgp);
#if CORE_USE_BRIDGE_MODE
                 mgmt_topic.forward_copy(*msgp, deadline);
#endif // CORE_USE_BRIDGE_MODE
                 mgmt_sub.release(*msgp);
                 break;
              }
              case MgmtMsg::SUBSCRIBE_RESPONSE:
              {
                 do_cmd_subscribe_response(*msgp);
#if CORE_USE_BRIDGE_MODE
                 mgmt_topic.forward_copy(*msgp, deadline);
#endif // CORE_USE_BRIDGE_MODE
                 mgmt_sub.release(*msgp);
                 break;
              }
              case MgmtMsg::STOP:
              {
                 if (0 == strncmp(module_namep, msgp->module.name, NamingTraits<Middleware>::MAX_LENGTH)) {
                    stop();
                 }

// [MARTINO]
//          mgmt_topic.notify_remotes(*msgp, deadline);
#if CORE_USE_BRIDGE_MODE
                 mgmt_topic.forward_copy(*msgp, deadline);
#endif // CORE_USE_BRIDGE_MODE
                 mgmt_sub.release(*msgp);
                 break;
              }
              case MgmtMsg::REBOOT:
              {
                 if (0 == strncmp(module_namep, msgp->module.name, NamingTraits<Middleware>::MAX_LENGTH)) {
                    preload_bootloader_mode(false);
                    reboot();
                 }

// [MARTINO]
//          mgmt_topic.notify_remotes(*msgp, deadline);
#if CORE_USE_BRIDGE_MODE
                 mgmt_topic.forward_copy(*msgp, deadline);
#endif // CORE_USE_BRIDGE_MODE
                 mgmt_sub.release(*msgp);
                 break;
              }
              case MgmtMsg::BOOTLOAD:
              {
                 if (0 == strncmp(module_namep, msgp->module.name, NamingTraits<Middleware>::MAX_LENGTH)) {
                    preload_bootloader_mode(true);
                    reboot();
                 }

// [MARTINO]
//          mgmt_topic.notify_remotes(*msgp, deadline);
#if CORE_USE_BRIDGE_MODE
                 mgmt_topic.forward_copy(*msgp, deadline);
#endif // CORE_USE_BRIDGE_MODE
                 mgmt_sub.release(*msgp);
                 break;
              }
              default:
              {
// [MARTINO]
//           mgmt_topic.notify_remotes(*msgp, deadline);
#if CORE_USE_BRIDGE_MODE
                 mgmt_topic.forward_copy(*msgp, deadline);
#endif // CORE_USE_BRIDGE_MODE
                 mgmt_sub.release(*msgp);
                 break;
              }
            } // switch
         }
      }

#if CORE_ITERATE_PUBSUB
      // Iterate publishers and subscribers
      else if (core::os::Time::now() - iter_lasttime >= core::os::Time::ms(ITER_TIMEOUT_MS)) {
         iter_lasttime = core::os::Time::now();

         if (!iter_nodes.is_valid()) {
            // Restart nodes iteration
            nodes.restart(iter_nodes);

            if (&*iter_nodes == &mgmt_node) {
               ++iter_nodes;
            }

            if (iter_nodes.is_valid()) {
               iter_nodes->get_publishers().restart(iter_publishers);
               iter_nodes->get_subscribers().restart(iter_subscribers);
            }

            // Tell it is alive
            if (mgmt_pub.alloc(msgp)) {
               Message::reset_payload(*msgp);
               msgp->type = MgmtMsg::ALIVE;
               strncpy(msgp->module.name, module_namep, NamingTraits<Middleware>::MAX_LENGTH);
               core::os::SysLock::acquire();
               msgp->module.flags.stopped = is_stopped() ? 1 : 0;
               core::os::SysLock::release();
               mgmt_pub.publish_remotely(*msgp);
            }
         } else if (iter_publishers.is_valid()) {
            // Advertise next publisher
            if (mgmt_pub.alloc(msgp)) {
               Message::reset_payload(*msgp);
               msgp->type = MgmtMsg::ADVERTISE;
               lists_lock.acquire();
               const Topic& topic = *iter_publishers->get_topic();
               msgp->pubsub.payload_size = static_cast<uint16_t>(topic.get_payload_size());
               strncpy(msgp->pubsub.topic, topic.get_name(), NamingTraits<Topic>::MAX_LENGTH);
               msgp->pubsub.queue_length = static_cast<uint16_t>(topic.get_max_queue_length());
               lists_lock.release();
               msgp->acquire();

               if (mgmt_topic.forward_copy(*msgp, mgmt_topic.compute_deadline())) {
                  ++iter_publishers;
               }

               mgmt_sub.release(*msgp);
            }
         } else if (iter_subscribers.is_valid()) {
            // Subscribe next subscriber
            if (mgmt_pub.alloc(msgp)) {
               Message::reset_payload(*msgp);
               msgp->type = MgmtMsg::SUBSCRIBE_REQUEST;
               core::os::SysLock::acquire();
               const Topic& topic = *iter_subscribers->get_topic();
               msgp->pubsub.payload_size = topic.get_payload_size();
               strncpy(msgp->pubsub.topic, topic.get_name(), NamingTraits<Topic>::MAX_LENGTH);
               msgp->pubsub.queue_length = topic.get_max_queue_length();
               core::os::SysLock::release();
               msgp->acquire();

               if (mgmt_topic.forward_copy(*msgp, mgmt_topic.compute_deadline())) {
                  ++iter_subscribers;
               }

               mgmt_sub.release(*msgp);
            }
         } else {
            ++iter_nodes;

            if (&*iter_nodes == &mgmt_node) {
               ++iter_nodes;
            }

            if (iter_nodes.is_valid()) {
               iter_nodes->get_publishers().restart(iter_publishers);
               iter_nodes->get_subscribers().restart(iter_subscribers);
            }
         }
      }
#endif // CORE_ITERATE_PUBSUB
   }
} // Middleware::do_mgmt_thread

void
Middleware::do_cmd_advertise(
   const MgmtMsg& msg
)
{
   Topic* topicp = find_topic(msg.pubsub.topic);

#if CORE_USE_BRIDGE_MODE
   CORE_ASSERT(msg.get_source() != NULL);

   if ((topicp != NULL) && topicp->has_subscribers()) {
#else // CORE_USE_BRIDGE_MODE
   CORE_ASSERT(transports.count() == 1);

   if ((topicp != NULL) && topicp->has_local_subscribers()) {
#endif // CORE_USE_BRIDGE_MODE
      {
         core::os::SysLock::Scope lock;

         if (mgmt_pub.get_topic() == NULL) {
            return;
         }
      }

      MgmtMsg* msgp;

      if (mgmt_pub.alloc(msgp)) {
         Message::reset_payload(*msgp);
         msgp->type = MgmtMsg::SUBSCRIBE_REQUEST;
         strncpy(msgp->pubsub.topic, topicp->get_name(), NamingTraits<Topic>::MAX_LENGTH);
         msgp->pubsub.payload_size = static_cast<uint16_t>(topicp->get_payload_size());
         core::os::SysLock::acquire();
         msgp->pubsub.queue_length = static_cast<uint16_t>(topicp->get_max_queue_length());
         core::os::SysLock::release();
         msgp->acquire();
#if CORE_USE_BRIDGE_MODE
         mgmt_topic.forward_copy(*msgp, topicp->compute_deadline());
#else // CORE_USE_BRIDGE_MODE
         mgmt_pub.publish_remotely(*msgp);
#endif // CORE_USE_BRIDGE_MODE
         mgmt_sub.release(*msgp);
      }
   }

#if CORE_USE_BRIDGE_MODE
   else {
      PubSubStep* curp;

      for (curp = pubsub_stepsp; curp != NULL; curp = curp->nextp) {
         if ((0 == strncmp(curp->topic, msg.pubsub.topic, NamingTraits<Topic>::MAX_LENGTH)) && (curp->type == MgmtMsg::ADVERTISE)) {
            // Advertisement already cached
            curp->timestamp = core::os::Time::now();
            break;
         }
      }

      if (curp != NULL) {
         curp               = alloc_pubsub_step();
         curp->type         = MgmtMsg::ADVERTISE;
         curp->transportp   = msg.get_source();
         curp->payload_size = msg.pubsub.payload_size;
         strncpy(curp->topic, msg.pubsub.topic, NamingTraits<Topic>::MAX_LENGTH);
      }
   }
#endif // CORE_USE_BRIDGE_MODE
} // Middleware::do_cmd_advertise

void
Middleware::do_cmd_subscribe_request(
   const MgmtMsg& msg
)
{
   Topic* topicp = find_topic(msg.pubsub.topic);

#if CORE_USE_BRIDGE_MODE
   CORE_ASSERT(msg.get_source() != NULL);

   if (topicp != NULL) {
#else // CORE_USE_BRIDGE_MODE
   CORE_ASSERT(transports.count() == 1);

   if ((topicp != NULL) && topicp->has_local_publishers()) {
#endif // CORE_USE_BRIDGE_MODE
      {
         core::os::SysLock::Scope lock;

         if (mgmt_pub.get_topic() == NULL) {
            return;
         }
      }

      MgmtMsg* msgp;

      if (mgmt_pub.alloc(msgp)) {
         Message::reset_payload(*msgp);
         msgp->type = MgmtMsg::SUBSCRIBE_RESPONSE;
         strncpy(msgp->pubsub.topic, topicp->get_name(), NamingTraits<Topic>::MAX_LENGTH);
         msgp->pubsub.payload_size = static_cast<uint16_t>(topicp->get_payload_size());
         core::os::SysLock::acquire();
         msgp->pubsub.queue_length = static_cast<uint16_t>(topicp->get_max_queue_length());
         core::os::SysLock::release();
         msgp->acquire();
#if CORE_USE_BRIDGE_MODE
         msg.get_source()->subscribe_cb(*topicp, msg.pubsub.queue_length, msgp->pubsub.raw_params);
         mgmt_topic.forward_copy(*msgp, topicp->compute_deadline());
#else // CORE_USE_BRIDGE_MODE
         transports.begin()->subscribe_cb(*topicp, msg.pubsub.queue_length, msgp->pubsub.raw_params);
         mgmt_pub.publish_remotely(*msgp);
#endif // CORE_USE_BRIDGE_MODE
         mgmt_sub.release(*msgp);
      }
   }

#if CORE_USE_BRIDGE_MODE
   else {
      PubSubStep* curp, * prevp = NULL;

      for (curp = pubsub_stepsp; curp != NULL; prevp = curp, curp = curp->nextp) {
         if (0 == strncmp(curp->topic, msg.pubsub.topic, NamingTraits<Topic>::MAX_LENGTH)) {
            if (curp->type == MgmtMsg::SUBSCRIBE_REQUEST) {
               // Subscription request already cached
               curp->timestamp = core::os::Time::now();
               break;
            } else if (curp->type == MgmtMsg::ADVERTISE) {
               // Subscription request matching advertisement
               if (prevp != NULL) {
                  prevp->nextp = curp->nextp;
               }

               char* namep = new char[NamingTraits < Topic > ::MAX_LENGTH];
               CORE_ASSERT(namep != NULL);
               strncpy(namep, curp->topic, NamingTraits<Topic>::MAX_LENGTH);
               topicp = touch_topic(namep, Message::get_type_size(curp->payload_size));
               CORE_ASSERT(topicp != NULL);
               pubsub_pool.free(curp);
               curp = NULL;
               break;
            }
         }
      }

      if (curp == NULL) {
         curp               = alloc_pubsub_step();
         curp->type         = MgmtMsg::SUBSCRIBE_REQUEST;
         curp->transportp   = msg.get_source();
         curp->payload_size = msg.pubsub.payload_size;
         core::os::SysLock::acquire();
         curp->queue_length = msg.pubsub.queue_length;
         core::os::SysLock::release();
         strncpy(curp->topic, msg.pubsub.topic, NamingTraits<Topic>::MAX_LENGTH);
      }
   }
#endif // CORE_USE_BRIDGE_MODE
} // Middleware::do_cmd_subscribe_request

void
Middleware::do_cmd_subscribe_response(
   const MgmtMsg& msg
)
{
#if CORE_USE_BRIDGE_MODE
   CORE_ASSERT(msg.get_source() != NULL);

   Topic* topicp = find_topic(msg.pubsub.topic);

   if (topicp != NULL) {
      msg.get_source()->advertise_cb(*topicp, msg.pubsub.raw_params);
   } else {
      PubSubStep* curp, * prevp = NULL;

      for (curp = pubsub_stepsp; curp != NULL; prevp = curp, curp = curp->nextp) {
         if ((0 == strncmp(curp->topic, msg.pubsub.topic, NamingTraits<Topic>::MAX_LENGTH)) && (curp->type == MgmtMsg::SUBSCRIBE_REQUEST)) {
            // Subscription response matching request
            if (prevp != NULL) {
               prevp->nextp = curp->nextp;
            }

            char* namep = new char[NamingTraits < Topic > ::MAX_LENGTH];
            CORE_ASSERT(namep != NULL);
            strncpy(namep, curp->topic, NamingTraits<Topic>::MAX_LENGTH);
            topicp = touch_topic(namep, Message::get_type_size(curp->payload_size));
            CORE_ASSERT(topicp != NULL);
            pubsub_pool.free(curp);
            curp = NULL;
            break;
         }
      }
   }

#else // CORE_USE_BRIDGE_MODE
   CORE_ASSERT(transports.count() == 1);

   Topic* topicp = find_topic(msg.pubsub.topic);

   if ((topicp != NULL) && topicp->has_local_subscribers()) {
      transports.begin()->advertise_cb(*topicp, msg.pubsub.raw_params);
   }
#endif // CORE_USE_BRIDGE_MODE
} // Middleware::do_cmd_subscribe_response

#if CORE_USE_BRIDGE_MODE

Middleware::PubSubStep*
Middleware::alloc_pubsub_step()
{
   PubSubStep* stepp = pubsub_pool.alloc();

   if (stepp != NULL) {
      memset(stepp, 0, sizeof(PubSubStep));
      stepp->timestamp = core::os::Time::now();
      stepp->nextp     = pubsub_stepsp;
      pubsub_stepsp    = stepp;
      return stepp;
   } else {
      core::os::Time now  = core::os::Time::now();
      PubSubStep* oldestp = pubsub_stepsp;

      for (stepp = pubsub_stepsp; stepp != NULL; stepp = stepp->nextp) {
         if ((now - stepp->timestamp) > (now - oldestp->timestamp)) {
            oldestp = stepp;
            break;
         }
      }

      CORE_ASSERT(oldestp != NULL);
      PubSubStep* old_nextp = oldestp->nextp;
      memset(oldestp, 0, sizeof(PubSubStep));
      oldestp->nextp     = old_nextp;
      oldestp->timestamp = core::os::Time::now();
      return oldestp;
   }
} // Middleware::alloc_pubsub_step
#endif // CORE_USE_BRIDGE_MODE


Middleware::Middleware(
   const char* module_namep,
   const char* bootloader_namep, // DAVIDE Remove...
   PubSubStep  pubsub_buf[],
   size_t      pubsub_length
)
   :
   module_namep(module_namep),
   lists_lock(false),
   mgmt_topic(MANAGEMENT_TOPIC_NAME, sizeof(MgmtMsg), false),
   mgmt_stackp(NULL),
   mgmt_stacklen(0),
   mgmt_threadp(NULL),
   mgmt_priority(core::os::Thread::LOWEST),
   mgmt_node("CORE_MGMT", false),
   mgmt_pub(),
   mgmt_sub(mgmt_queue_buf, MGMT_BUFFER_LENGTH, NULL),
#if CORE_IS_BOOTLOADER_BRIDGE
   boot_topic(BOOTLOADER_TOPIC_NAME, sizeof(BootMsg), false),
   bootmaster_topic(BOOTLOADER_MASTER_TOPIC_NAME, sizeof(BootMasterMsg), false),
#endif
#if CORE_USE_BRIDGE_MODE
   pubsub_stepsp(NULL),
   pubsub_pool(pubsub_buf, pubsub_length),
#endif
   stopped(false),
   num_running_nodes(0)
{
   CORE_ASSERT(is_identifier(module_namep, NamingTraits<Middleware>::MAX_LENGTH));
   (void)bootloader_namep;
   (void)pubsub_buf;
   (void)pubsub_length;
}

void
Middleware::mgmt_threadf(
   core::os::Thread::Argument
)
{
   instance.do_mgmt_thread();
   chThdExitS(core::os::Thread::OK);
}

NAMESPACE_CORE_MW_END
