/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/SubscriberExtBuf.hpp>
#include <core/os/Time.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;
class Node;

template <typename MessageType, unsigned QUEUE_LENGTH>
class Subscriber:
   public SubscriberExtBuf<MessageType>
{
   friend class Node;

public:
   typedef typename SubscriberExtBuf<MessageType>::Callback Callback;

private:
   MessageType  msgpool_buf[QUEUE_LENGTH];
   MessageType* queue_buf[QUEUE_LENGTH];

public:
   Subscriber(
      Callback callback = NULL
   );
   ~Subscriber();
};


template <typename MT, unsigned QL>
inline
Subscriber<MT, QL>::Subscriber(
   Callback callback
)
   :
   SubscriberExtBuf<MT>(queue_buf, QL, callback)
{}


template <typename MT, unsigned QL>
inline
Subscriber<MT, QL>::~Subscriber() {}


NAMESPACE_CORE_MW_END
