/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>

#include <Core/MW/Thread.hpp>
#include <Core/MW/Condition.hpp>
#include <Core/MW/Mutex.hpp>
#include <Core/MW/Node.hpp>
#include <Core/MW/CoreConfiguration.hpp>

#include <Core/MW/ICoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

class CoreNode:
   public ICoreNode,
   protected Core::MW::Node
{
public:
   virtual ~CoreNode() {}

   CoreNode(
      const char*                name,
      Core::MW::Thread::Priority priority = Core::MW::Thread::PriorityEnum::NORMAL
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
   Core::MW::Thread::Priority _priority;

private:
   State
   _state();

   void
   _state(
      State state
   );

   void
   _run();


   Core::MW::Thread* _runner;

   Core::MW::Mutex     _mutex;
   Core::MW::Condition _condition;

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
   mutable Core::MW::StaticList<CoreNode>::Link link;
};

inline bool
CoreNode::mustLoop()
{
   return _mustLoop;
}

NAMESPACE_CORE_MW_END
