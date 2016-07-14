/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreNodeManager.hpp>

NAMESPACE_CORE_MW_BEGIN

CoreNodeManager::CoreNodeManager()
{
   // TODO Auto-generated constructor stub
}

CoreNodeManager::~CoreNodeManager()
{
   // TODO Auto-generated destructor stub
}

void
CoreNodeManager::add(
   const CoreNode& node
)
{
   _nodes.link(node.link);
}

#define FOREACH_NODE(_x_) \
   for (core::mw::StaticList<CoreNode>::Iterator _x_ = _nodes.begin(); _x_ != _nodes.end(); _x_++)

bool
CoreNodeManager::syncronize(
   ICoreNode::Action action,
   ICoreNode::State  state
)
{
   bool done  = true;
   bool error = false;

   FOREACH_NODE(node) {
      node->execute(action);
   }

   do {
      done  = true;
      error = false;

      core::mw::Thread::yield();

      FOREACH_NODE(node) {
         ICoreNode::State state2 = node->state();

         done  &= state == state2;
         error |= state == CoreNode::State::ERROR;
      }
   } while (!done && !error);

   return done && !error;
} // CoreNodeManager::syncronize

bool
CoreNodeManager::setup()
{
   bool success = true;

   FOREACH_NODE(node) {
      success &= node->setup();
   }

   return success;
}

bool
CoreNodeManager::run()
{
   bool success = true;

   success &= syncronize(ICoreNode::Action::INITIALIZE, ICoreNode::State::INITIALIZED);
   success &= syncronize(ICoreNode::Action::CONFIGURE, ICoreNode::State::CONFIGURED);
   success &= syncronize(ICoreNode::Action::PREPARE_HW, ICoreNode::State::HW_READY);
   success &= syncronize(ICoreNode::Action::PREPARE_MW, ICoreNode::State::IDLE);
   success &= syncronize(ICoreNode::Action::START, ICoreNode::State::LOOPING);

   return success;
}

bool
CoreNodeManager::stop()
{
   bool success = true;

   success &= syncronize(ICoreNode::Action::STOP, ICoreNode::State::IDLE);
   success &= syncronize(ICoreNode::Action::FINALIZE, ICoreNode::State::SET_UP);

   return success;
}

bool
CoreNodeManager::teardown()
{
   bool success = true;

   FOREACH_NODE(node) {
      success &= node->teardown();
   }

   return success;
}

bool
CoreNodeManager::isOk()
{
   bool success = true;

   FOREACH_NODE(node) {
      success &= node->state() != ICoreNode::State::ERROR;
   }

   return success;
}

NAMESPACE_CORE_MW_END
