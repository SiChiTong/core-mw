/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

CoreNode::CoreNode(
    const char*                name,
    core::os::Thread::Priority priority
) :
    _node(name, false),
    _workingAreaSize(0),
    _priority(priority),
    _runner(nullptr),
    _mustRun(false),
    _mustLoop(false),
    _mustTeardown(false),
    link(*this)
{}

CoreNode::CoreNode() :
    _node("", false),
    _workingAreaSize(0),
    _priority(core::os::Thread::NORMAL),
    _runner(nullptr),
    _mustRun(false),
    _mustLoop(false),
    _mustTeardown(false),
    link(*this)
{}

const char*
CoreNode::name()
{
    return _node.get_name();
}

bool
CoreNode::setup()
{
    _mutex.acquire();

    if (_state() == State::NONE) {
        _mustRun = true;
        _runner  = core::os::Thread::create_heap(nullptr, _workingAreaSize, core::os::Thread::PriorityEnum::NORMAL,
                                                 [](void* arg) {
            reinterpret_cast<CoreNode*>(arg)->_run(); // execute the thread code in the thread
        }, this, _node.get_name());

        if (_runner != nullptr) {
            while (_currentState != State::SET_UP) {
                // wait for the thread to spawn...
                core::os::Thread::yield();
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

    core::os::Thread::join(*_runner);

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
CoreNode::state() const
{
    return _state();
}

ICoreNode::State
CoreNode::_state() const
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
    _runner->set_name(_node.get_name());

    while (_mustRun) {
        if (core::os::Thread::should_terminate()) {
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
    if (onInitialize()) {
        _state(State::INITIALIZED);
    } else {
        _doError();
    }
}

inline void
CoreNode::_doConfigure()
{
    if (onConfigure()) {
        _state(State::CONFIGURED);
    } else {
        _doError();
    }
}

inline void
CoreNode::_doPrepareHW()
{
    if (onPrepareHW()) {
        _state(State::HW_READY);
    } else {
        _doError();
    }
}

inline void
CoreNode::_doPrepareMW()
{
    _node.set_enabled(true); // Enable event dispatching for this Node

    if (onPrepareMW()) {
        _state(State::MW_READY);
        _state(State::IDLE);
    } else {
        _doError();
    }
}

inline void
CoreNode::_doStart()
{
    if (onStart()) {
        _mustLoop = true;
        _state(State::LOOPING);
    } else {
        _doError();
    }
}

inline void
CoreNode::_doLoop()
{
    core::os::Thread::set_priority(_priority);

    while (_mustLoop) {
        if (core::os::Thread::should_terminate()) {
            teardown();
        } else {
            if (!onLoop()) {
                _doError();
                _mustLoop = false;
            }
        }
    }

    core::os::Thread::set_priority(core::os::Thread::PriorityEnum::NORMAL);

    _state(State::STOPPING);
} // CoreNode::_doLoop

inline void
CoreNode::_doStop()
{
    if (onStop()) {
        _mustLoop = false;
        _state(State::IDLE);
    } else {
        _doError();
    }
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
    _node.set_enabled(false); // Disable event dispatching for this Node

    if (onFinalize()) {
        _state(State::SET_UP);
    } else {
        _doError();
    }
}

bool
CoreNode::spin(
    const core::os::Time& timeout
)
{
    return _node.spin(timeout, this);
}

NAMESPACE_CORE_MW_END
