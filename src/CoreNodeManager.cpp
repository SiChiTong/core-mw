/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreNodeManager.hpp>

NAMESPACE_CORE_MW_BEGIN

CoreNodeManager::CoreNodeManager()
{}

CoreNodeManager::~CoreNodeManager()
{}

void
CoreNodeManager::add(
   const CoreNode& node
)
{
   _nodes.link(node.link);
}

bool
CoreNodeManager::syncronize(
   ICoreNode::Action action,
   ICoreNode::State  state
)
{
   bool done  = true;
   bool error = false;

   for (CoreNode& node : _nodes) {
      node.execute(action);
   }


   do {
      done  = true;
      error = false;

      core::os::Thread::yield();

      for (const CoreNode& node : _nodes) {
         ICoreNode::State state2 = node.state();

         done  &= state == state2;
         error |= (state2 == ICoreNode::State::ERROR);
      }
   } while (!done && !error);

   return done && !error;
} // CoreNodeManager::syncronize

bool
CoreNodeManager::setup()
{
   bool success = true;

   for (CoreNode& node : _nodes) {
      success &= node.setup();
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

   for (CoreNode& node : _nodes) {
      success &= node.teardown();
   }

   return success;
}

bool
CoreNodeManager::isOk()
{
   bool success = true;

   for (const CoreNode& node : _nodes) {
      success &= node.state() != ICoreNode::State::ERROR;
   }

   return success;
}

NAMESPACE_CORE_MW_END
