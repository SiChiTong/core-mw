/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/namespace.hpp>

NAMESPACE_CORE_MW_BEGIN

class ICoreNode
{
public:
   enum class State {
      NONE, // Not set up
      SET_UP, // wait
      INITIALIZING, // doInitialize
      INITIALIZED, // wait
      CONFIGURING, // doConfigure
      CONFIGURED, // wait
      PREPARING_HW, // doPrepareHW
      HW_READY, // wait
      PREPARING_MW, // doPrepareMW
      MW_READY, // wait
      IDLE, // wait
      STARTING, // doStart
      LOOPING, // doLoop
      STOPPING, // doStop
      FINALIZING, // doFinalize
      ERROR, // wait forever
      TEARING_DOWN // killing
   };

   enum class Action {
      INITIALIZE, CONFIGURE, PREPARE_HW, PREPARE_MW, START, STOP, FINALIZE
   };

   virtual bool
   setup() = 0;

   virtual bool
   teardown() = 0;

   virtual bool
   execute(
      Action what
   ) = 0;

   virtual State
   state() = 0;


   virtual ~ICoreNode() {}

protected:
   virtual bool
   onInitialize() = 0;

   virtual bool
   onConfigure() = 0;

   virtual bool
   onPrepareHW() = 0;

   virtual bool
   onPrepareMW() = 0;

   virtual bool
   onStart() = 0;

   virtual bool
   onLoop() = 0;

   virtual bool
   onStop() = 0;

   virtual bool
   onError() = 0;

   virtual bool
   onFinalize() = 0;


protected:
   ICoreNode() :
      _currentState(State::NONE)
   {}

   State _currentState;
};

NAMESPACE_CORE_MW_END
