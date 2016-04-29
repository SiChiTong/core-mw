/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>
#include <Core/MW/common.hpp>
#include <Core/MW/RemotePublisher.hpp>
#include <Core/MW/StaticList.hpp>

#include "rtcan.h"

NAMESPACE_CORE_MW_BEGIN


class RTCANPublisher:
   public RemotePublisher
{
public:
   rtcan_msg_t rtcan_header; // FIXME should be private

   RTCANPublisher(
      Transport& transport
   );
   virtual
   ~RTCANPublisher();
};


NAMESPACE_CORE_MW_END
