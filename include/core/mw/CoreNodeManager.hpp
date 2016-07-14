/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

#include <core/mw/StaticList.hpp>
#include <core/mw/CoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

class CoreNodeManager
{
public:
   CoreNodeManager();
   virtual
   ~CoreNodeManager();

   void
   add(
      const CoreNode& node
   );

   bool
   setup();

   bool
   run();

   bool
   stop();

   bool
   teardown();

   bool
   isOk();


private:
   core::mw::StaticList<CoreNode> _nodes;

   bool
   syncronize(
      ICoreNode::Action action,
      ICoreNode::State  state
   );
};

NAMESPACE_CORE_MW_END
