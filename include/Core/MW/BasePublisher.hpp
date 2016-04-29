/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/Message.hpp>
#include <Core/MW/MgmtMsg.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;
class Topic;


class BasePublisher:
   private Uncopyable
{
private:
   Topic* topicp;

public:
   Topic*
   get_topic() const;

   void
   notify_advertised(
      Topic& topic
   );

   bool
   alloc_unsafe(
      Message*& msgp
   );

   bool
   publish_unsafe(
      Message& msg
   );

   bool
   publish_locally_unsafe(
      Message& msg
   );

   bool
   publish_remotely_unsafe(
      Message& msg
   );

   bool
   alloc(
      Message*& msgp
   );

   bool
   publish(
      Message& msg
   );

   bool
   publish_locally(
      Message& msg
   );

   bool
   publish_remotely(
      Message& msg
   );


protected:
   BasePublisher();
   virtual
   ~BasePublisher() = 0;

public:
   static bool
   has_topic(
      const BasePublisher& pub,
      const char*          namep
   );
};


NAMESPACE_CORE_MW_END

#include <Core/MW/Topic.hpp>

NAMESPACE_CORE_MW_BEGIN


inline
Topic*
BasePublisher::get_topic() const
{
   return topicp;
}

inline
void
BasePublisher::notify_advertised(
   Topic& topic
)
{
   CORE_ASSERT(topicp == NULL);

   topicp = &topic;
}

inline
bool
BasePublisher::alloc_unsafe(
   Message*& msgp
)
{
   CORE_ASSERT(topicp != NULL);

   msgp = topicp->alloc_unsafe();
   return msgp != NULL;
}

inline
bool
BasePublisher::alloc(
   Message*& msgp
)
{
   SysLock::acquire();
   bool success = alloc_unsafe(msgp);
   SysLock::release();

   return success;
}

inline
bool
BasePublisher::publish_locally(
   Message& msg
)
{
   CORE_ASSERT(topicp != NULL);

   return topicp->notify_locals(msg, Time::now());
}

inline
bool
BasePublisher::publish_remotely(
   Message& msg
)
{
   CORE_ASSERT(topicp != NULL);
   CORE_ASSERT(static_cast<MgmtMsg*>(&msg)->type != 0);

   return topicp->notify_remotes(msg, Time::now());
}

inline
bool
BasePublisher::has_topic(
   const BasePublisher& pub,
   const char*          namep
)
{
   return pub.topicp != NULL && Topic::has_name(*pub.topicp, namep);
}

NAMESPACE_CORE_MW_END
