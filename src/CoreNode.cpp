/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <Core/MW/CoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

CoreNode::CoreNode(
   const char*                name,
   Core::MW::Thread::Priority priority
) :
   Core::MW::Node(name, false),
   _workingAreaSize(0),
   _priority(priority),
   _runner(nullptr),
   _mustRun(false),
   _mustLoop(false),
   _mustTeardown(false),
   link(*this)
{}

CoreNode::CoreNode() :
   Core::MW::Node("", false),
   _workingAreaSize(0),
   _priority(Core::MW::Thread::NORMAL),
   _runner(nullptr),
   _mustRun(false),
   _mustLoop(false),
   _mustTeardown(false),
   link(*this)
{}

const char*
CoreNode::name()
{
   return get_name();
}

bool
CoreNode::setup()
{
   _mutex.acquire();

   if (_state() == State::NONE) {
      _mustRun = true;
      _runner  = Core::MW::Thread::create_heap(NULL, THD_WORKING_AREA_SIZE(_workingAreaSize), Core::MW::Thread::get_priority(),
                                               [](void* arg) {
         reinterpret_cast<CoreNode*>(arg)->_run(); // execute the thread code in the thread
      }, this, get_name());

      if (_runner != nullptr) {
         while (_currentState != State::SET_UP) {
            // wait for the thread to spawn...
            Core::MW::Thread::yield();
         }
      } else {
         _currentState = State::NONE;
      }

      _mutex.release();

      return _runner != nullptr;
   } else {
      _mutex.release();
      return false;
   }
} // CoreNode::setup

bool
CoreNode::teardown()
{
   _mustRun      = false;
   _mustLoop     = false;
   _mustTeardown = true;

   _state(State::TEARING_DOWN);

   Core::MW::Thread::join(*_runner);

   return true;
}

bool
CoreNode::execute(
   Action what
)
{
   if (_runner == nullptr) {
      return false;
   }

   bool executed = false;

   // asynchronously request to stop looping.
   if (what == Action::STOP) {
      if (_state() == State::LOOPING) {
         _mustLoop = false;

         executed = true;
      }
   } else {
      _mutex.acquire();

      switch (what) {
        case Action::INITIALIZE:

           if (_state() == State::SET_UP) {
              _state(State::INITIALIZING);
              executed = true;
           }

           break;
        case Action::CONFIGURE:

           if (_state() == State::INITIALIZED) {
              _state(State::CONFIGURING);
              executed = true;
           }

           break;
        case Action::PREPARE_HW:

           if (_state() == State::CONFIGURED) {
              _state(State::PREPARING_HW);
              executed = true;
           }

           break;
        case Action::PREPARE_MW:

           if (_state() == State::HW_READY) {
              _state(State::PREPARING_MW);
              executed = true;
           }

           break;
        case Action::START:

           if (_state() == State::IDLE) {
              _state(State::STARTING);
              executed = true;
           }

           break;
        case Action::FINALIZE:

           if (_state() == State::IDLE) {
              _state(State::FINALIZING);
              executed = true;
           }

           break;
        default:
           executed = false; // unreachable?
           break;
      } // switch

      _mutex.release();
   }

   return executed;
} // CoreNode::execute

ICoreNode::State
CoreNode::state()
{
   return _state();
}

ICoreNode::State
CoreNode::_state()
{
   return _currentState;
}

void
CoreNode::_state(
   State state
)
{
   if (_mustTeardown) {
      // There's no escape from a teardown request!
      _currentState = State::TEARING_DOWN;
   } else {
      if (_currentState != State::ERROR) {
         // Make sure not to hide an Error condition.
         _currentState = state;
      }
   }

   _condition.signal();
}

void
CoreNode::_run()
{
   _currentState = State::SET_UP;
   _runner->set_name(get_name());

   while (_mustRun) {
      if (Thread::should_terminate()) {
         teardown();
      }

      _mutex.acquire();

      switch (_state()) {
        case State::INITIALIZING:
           _doInitialize();
           break;
        case State::CONFIGURING:
           _doConfigure();
           break;
        case State::PREPARING_HW:
           _doPrepareHW();
           break;
        case State::PREPARING_MW:
           _doPrepareMW();
           break;
        case State::STARTING:
           _doStart();
           break;
        case State::LOOPING:
           _doLoop();
           break;
        case State::STOPPING:
           _doStop();
           break;
        case State::FINALIZING:
           _doFinalize();
           break;
        case State::TEARING_DOWN:
           break;
        default:
           _condition.wait();
      } // switch
      _mutex.release();
   }

   _currentState = State::NONE;
   _runner       = nullptr;
} // CoreNode::_run

inline bool
CoreNode::onInitialize()
{
   return true;
}

inline bool
CoreNode::onConfigure()
{
   return true;
}

inline bool
CoreNode::onPrepareHW()
{
   return true;
}

inline bool
CoreNode::onPrepareMW()
{
   return true;
}

inline bool
CoreNode::onStart()
{
   return true;
}

inline bool
CoreNode::onLoop()
{
   return true;
}

inline bool
CoreNode::onStop()
{
   return true;
}

inline bool
CoreNode::onError()
{
   return true;
}

inline bool
CoreNode::onFinalize()
{
   return true;
}

inline void
CoreNode::_doInitialize()
{
   if (!onInitialize()) {
      _doError();
   }

   _state(State::INITIALIZED);
}

inline void
CoreNode::_doConfigure()
{
   if (!onConfigure()) {
      _doError();
   }

   _state(State::CONFIGURED);
}

inline void
CoreNode::_doPrepareHW()
{
   if (!onPrepareHW()) {
      _doError();
   }

   _state(State::HW_READY);
}

inline void
CoreNode::_doPrepareMW()
{
   set_enabled(true); // Enable event dispatching for this Node

   if (!onPrepareMW()) {
      _doError();
   }

   _state(State::MW_READY);
   _state(State::IDLE);
}

inline void
CoreNode::_doStart()
{
   if (!onStart()) {
      _doError();
   }

   _mustLoop = true;
   _state(State::LOOPING);
}

inline void
CoreNode::_doLoop()
{
   Core::MW::Thread::set_priority(_priority);

   while (_mustLoop) {
      if (Thread::should_terminate()) {
         teardown();
      } else {
         if (!onLoop()) {
            _doError();
            _mustLoop = false;
         }
      }
   }

   _state(State::STOPPING);
}

inline void
CoreNode::_doStop()
{
   if (!onStop()) {
      _doError();
   }

   _mustLoop = false;
   _state(State::IDLE);
}

inline void
CoreNode::_doError()
{
   onError();
   _state(State::ERROR);
}

inline void
CoreNode::_doFinalize()
{
   set_enabled(false); // Disable event dispatching for this Node

   if (!onFinalize()) {
      _doError();
   }

   _state(State::SET_UP);
}

NAMESPACE_CORE_MW_END
