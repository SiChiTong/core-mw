/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/ArrayQueue.hpp>

NAMESPACE_CORE_MW_BEGIN

class Message;


class MessagePtrQueue:
   public ArrayQueue<Message*>
{
public:
   MessagePtrQueue(
      Message* arrayp[],
      size_t   length
   );
};


NAMESPACE_CORE_MW_END
