/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/LocalPublisher.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;


template <typename MessageType>
class Publisher:
   public LocalPublisher
{
public:
   bool
   alloc(
      MessageType*& msgp
   );

   bool
   publish(
      MessageType& msg
   );

   bool
   publish(
      MessageType* msg
   );


public:
   Publisher();
   ~Publisher();
};


template <typename MessageType>
//inline // DAVIDE
bool
Publisher<MessageType>::alloc(
   MessageType*& msgp
)
{
   static_cast_check<MessageType, Message>();
   return BasePublisher::alloc(reinterpret_cast<Message*&>(msgp));
}

template <typename MessageType>
bool
Publisher<MessageType>::publish(
   MessageType& msg
)
{
   static_cast_check<MessageType, Message>();
   return BasePublisher::publish(static_cast<Message&>(msg));
}

template <typename MessageType>
inline
bool
Publisher<MessageType>::publish(
   MessageType* msg
)
{
   static_cast_check<MessageType, Message>();
   return BasePublisher::publish(static_cast<Message&>(*msg));
}

template <typename MessageType>
inline
Publisher<MessageType>::Publisher()
{
   static_cast_check<MessageType, Message>();
}

template <typename MessageType>
inline
Publisher<MessageType>::~Publisher()
{
   static_cast_check<MessageType, Message>();
}

NAMESPACE_CORE_MW_END
