/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
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
 * Each node is a thread.
 *
 * \todo Add some explainations about the thread thing
 *
 */
class CoreNode:
    public ICoreNode
{
public:
    virtual ~CoreNode() {}

    /*! \brief Constructor
     *
     * \warning The specified priority is used only during State::LOOPING state. In all the other state the node priority is core::os::Thread::PriorityEnum::NORMAL
     */
    CoreNode(
        const char*                name, //!< [in] name of the node
        core::os::Thread::Priority priority = core::os::Thread::PriorityEnum::NORMAL //!< [in] priority for the State::LOOPING state
    );

    /*! \brief Constructor
     *
     * \warning The specified priority is used only during State::LOOPING state. In all the other state the node priority is core::os::Thread::PriorityEnum::NORMAL
     */
    CoreNode(
        const char*                name, //!< [in] name of the node
        core::os::Thread::Priority priority, //!< [in] priority for the State::LOOPING state
        std::size_t                threadStackSize //!< [in] size of the thread stack
    );

    /*! \brief Create the node thread
     *
     * If the thread has been created, the caller is suspended until the node reaches a State::SET_UP state.
     *
     * \pre The node must be in State::NONE state
     * \post The node is either in State::SET_UP or State::NONE states
     *
     * \retval true node is running
     * \retval false it was not possible to create the thread
     */
    bool
    setup();


    /*! \brief Tear down the node
     *
     * The caller is suspended until the node is down (i.e.: the thread is finished)
     *
     * \post The node is in State::NONE state
     *
     * \return succes, and as it cannot fail, it always return true.
     * \retval true always
     */
    bool
    teardown();


    /*! \brief Executes an action
     *
     * Depending on the state the node is into, only some actions are possible, as explained in \ref ICoreNode
     *
     * \retval true the action was admissible
     * \retval false the node is not running, or the action was not admissible
     *
     * \see ICoreNode
     */
    bool
    execute(
        Action what //!< [in] the action we want to perform on the node
    );

    State
    state() const;


    /*! \brief Name of the node
     *
     * \return name of the node
     */
    const char*
    name();


    /*! \brief Advertise a publisher
     */
    template <typename MT>
    bool
    advertise(
        Publisher<MT>&        pub, //!< [in] publisher
        const char*           namep, //!< [in] name of the topic
        const core::os::Time& publish_timeout = core::os::Time::INFINITE //!< [in] timeout
    );


    /*! \brief Subscribe
     */
    template <typename MT, unsigned QL>
    bool
    subscribe(
        Subscriber<MT, QL>& sub, //!< [in] subscriber
        const char* namep //!< [in] name of the topic
    );


    /*! \brief Spin
     *
     * This method calls the subscriber registered callbacks
     */
    bool
    spin(
        const core::os::Time& timeout = core::os::Time::INFINITE //!< [in] timeout
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


    core::mw::Node _node;

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

/* ------------------------------------------------------------------------- */

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
