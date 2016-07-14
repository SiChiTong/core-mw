/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

#include <core/mw/Thread.hpp>
#include <core/mw/Condition.hpp>
#include <core/mw/Mutex.hpp>
#include <core/mw/Node.hpp>

#include <core/mw/ICoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

class CoreNode:
   public ICoreNode,
   protected core::mw::Node
{
public:
   virtual ~CoreNode() {}

   CoreNode(
      const char*                name,
      core::mw::Thread::Priority priority = core::mw::Thread::PriorityEnum::NORMAL
   );

   bool
   setup();

   bool
   teardown();

   bool
   execute(
      Action what
   );

   State
   state();

   const char*
   name();


protected:
   virtual bool
   onInitialize();

   virtual bool
   onConfigure();

   virtual bool
   onPrepareHW();

   virtual bool
   onPrepareMW();

   virtual bool
   onStart();

   virtual bool
   onLoop();

   virtual bool
   onStop();

   virtual bool
   onError();

   virtual bool
   onFinalize();

   bool
   mustLoop();


   CoreNode();

   std::size_t _workingAreaSize;
   core::mw::Thread::Priority _priority;

private:
   State
   _state();

   void
   _state(
      State state
   );

   void
   _run();


   core::mw::Thread* _runner;

   core::mw::Mutex     _mutex;
   core::mw::Condition _condition;

   bool _mustRun;
   bool _mustLoop;
   bool _mustTeardown;

   void
   _doInitialize();

   void
   _doConfigure();

   void
   _doPrepareHW();

   void
   _doPrepareMW();

   void
   _doStart();

   void
   _doLoop();

   void
   _doStop();

   void
   _doError();

   void
   _doFinalize();


public:
   mutable core::mw::StaticList<CoreNode>::Link link;
};

inline bool
CoreNode::mustLoop()
{
   return _mustLoop;
}

NAMESPACE_CORE_MW_END
