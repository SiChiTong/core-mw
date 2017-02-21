/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

#include <core/os/Thread.hpp>
#include <core/os/Condition.hpp>
#include <core/os/Mutex.hpp>
#include <core/mw/Node.hpp>

#include <core/mw/ICoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

/*! \brief Base class for all managed nodes
 *
 */
class CoreNode:
    public ICoreNode
{
public:
    virtual ~CoreNode() {}

    CoreNode(
        const char*                name,
        core::os::Thread::Priority priority = core::os::Thread::PriorityEnum::NORMAL
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
    state() const;

    const char*
    name();


    /*! \brief Advertise a publisher
     *
     */
    template <typename MT>
    bool
    advertise(
        Publisher<MT>&        pub, //!< publisher
        const char*           namep, //!< name of the topic
        const core::os::Time& publish_timeout = core::os::Time::INFINITE //!< timeout
    );


    /*! \brief Subscribe
     *
     */
    template <typename MT, unsigned QL>
    bool
    subscribe(
        Subscriber<MT, QL>& sub, //!< subscriber
        const char* namep //!< name of the topic
    );


    /*! \brief Dispatch
     *
     */
    bool
    spin(
        const core::os::Time& timeout = core::os::Time::INFINITE //!< timeout
    );


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

private:
    core::mw::Node _node;

protected:
    std::size_t _workingAreaSize;
    core::os::Thread::Priority _priority;

private:
    State
    _state() const;

    void
    _state(
        State state
    );

    void
    _run();


    core::os::Thread* _runner;

    core::os::Mutex     _mutex;
    core::os::Condition _condition;

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

template <typename MT>
inline bool
CoreNode::advertise(
    Publisher<MT>&        pub,
    const char*           namep,
    const core::os::Time& publish_timeout
)
{
    return _node.advertise(pub, namep, publish_timeout);
}

template <typename MT, unsigned QL>
inline bool
CoreNode::subscribe(
    Subscriber<MT, QL>& sub,
    const char* namep
)
{
    return _node.subscribe(sub, namep);
}

NAMESPACE_CORE_MW_END
