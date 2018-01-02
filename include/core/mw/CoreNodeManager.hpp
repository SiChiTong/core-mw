/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

#include <core/mw/StaticList.hpp>
#include <core/mw/CoreNode.hpp>

NAMESPACE_CORE_MW_BEGIN

/*! \brief CoreNode manager.
 *
 * Manages CoreNode objects, syncronizing their states.
 */
class CoreNodeManager
{
public:
    CoreNodeManager();

    virtual
    ~CoreNodeManager();

    /*! \brief Adds a CoreNode to the managed objects.
   *
   */
    void
    add(
        const CoreNode& node
    );


    /*! \brief Setup the nodes.
     *
     * For each object, it calls setup()
     *
     * \retval true all the nodes were successfully setup.
     */
    bool
    setup();


    /*! \brief Start the nodes.
     *
     * For each object, it calls: execute(INITIALIZE), execute(CONFIGURE), execute(PREPARE_HW), execute(PREPARE_MW), execute(START), syncronizing all the objects to the same state.
     *
     * \retval true all the nodes have successfully reached ICoreNode::State::RUNNING state.
     */
    bool
    run();


    /*! \brief Stop the nodes.
     *
     * For each object, it calls: execute(STOP), execute(FINALIZE), syncronizing all the objects to the same state.
     *
     * \retval true all the nodes have successfully reached ICoreNode::State::SET_UP state.
     */
    bool
    stop();


    /*! \brief Teardown the nodes.
     *
     * For each object, it calls teardown()
     *
     * \retval true all the nodes were successfully teared down.
     */
    bool
    teardown();


    /*! \brief Check the state of the nodes.
     *
     * For each object, it checks if the state is different than ICoreNode::State::ERROR
     *
     * \retval true all the nodes are working properly
     */
    bool
    areOk();


private:
    core::mw::StaticList<CoreNode> _nodes;

    bool
    syncronize(
        ICoreNode::Action action,
        ICoreNode::State  state
    );
};

NAMESPACE_CORE_MW_END
