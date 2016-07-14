/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/mw/common.hpp>
#include <core/mw/BasePublisher.hpp>
#include <core/mw/StaticList.hpp>

NAMESPACE_CORE_MW_BEGIN


class LocalPublisher:
   public BasePublisher
{
   friend class Node;

private:
   mutable StaticList<LocalPublisher>::Link by_node;

protected:
   LocalPublisher();
   virtual
   ~LocalPublisher() = 0;
};


NAMESPACE_CORE_MW_END
