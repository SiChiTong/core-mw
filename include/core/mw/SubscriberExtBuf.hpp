/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/LocalSubscriber.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;


template <typename MessageType>
class SubscriberExtBuf:
   public LocalSubscriber
{
public:
   typedef bool (* Callback)(
      const MessageType& msg,
      Node*              node
   );

public:
   Callback
   get_callback() const;

   void
   set_callback(
      Callback callback
   );

   bool
   fetch(
      MessageType*& msgp
   );

   bool
   fetch(
      MessageType*&   msgp,
      core::os::Time& timestamp
   );

   bool
   release(
      MessageType& msg
   );


public:
   SubscriberExtBuf(
      MessageType* queue_buf[],
      size_t       queue_length,
      Callback     callback = NULL
   );
   ~SubscriberExtBuf();
};


template <typename MT>
inline
bool
SubscriberExtBuf<MT>::fetch(
   MT*& msgp
)
{
   return LocalSubscriber::fetch(
      reinterpret_cast<Message*&>(msgp));
}

template <typename MT>
inline
bool
SubscriberExtBuf<MT>::fetch(
   MT*&            msgp,
   core::os::Time& timestamp
)
{
   static_cast_check<MT, Message>();
   return LocalSubscriber::fetch(
      reinterpret_cast<Message*&>(msgp), timestamp
   );
}

template <typename MT>
inline
typename SubscriberExtBuf<MT>::Callback
SubscriberExtBuf<MT>::get_callback() const
{
   return reinterpret_cast<Callback>(LocalSubscriber::get_callback());
}

template <typename MT>
inline
void
SubscriberExtBuf<MT>::set_callback(
   SubscriberExtBuf<MT>::Callback callback
)
{
   LocalSubscriber::set_callback(reinterpret_cast<LocalSubscriber::Callback>(callback));
}

template <typename MT>
inline
bool
SubscriberExtBuf<MT>::release(
   MT& msg
)
{
   static_cast_check<MT, Message>();
   return LocalSubscriber::release(static_cast<Message&>(msg));
}

template <typename MT>
inline
SubscriberExtBuf<MT>::SubscriberExtBuf(
   MT*      queue_buf[],
   size_t   queue_length,
   Callback callback
)
   :
   LocalSubscriber(reinterpret_cast<Message**>(queue_buf), queue_length,
                   reinterpret_cast<LocalSubscriber::Callback>(callback))
{
   static_cast_check<MT, Message>();
}

template <typename MT>
inline
SubscriberExtBuf<MT>::~SubscriberExtBuf()
{
   static_cast_check<MT, Message>();
}

NAMESPACE_CORE_MW_END
