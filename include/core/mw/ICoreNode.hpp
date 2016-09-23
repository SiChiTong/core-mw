/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

NAMESPACE_CORE_MW_BEGIN

/*! \brief ICoreNode interface
 *
 * TODO: Put a graph of the state machine
 */
class ICoreNode
{
public:
   /*! \brief CoreNode states
    *
    * Interface to be implemented by a node that must be managed by a CoreManager container.
    */
   enum class State {
      NONE, //!< Not set up
      SET_UP, //!< Set up wait [wait for ICoreNode::execute Action::INITIALIZE]
      INITIALIZING, //!< Initializing [call ICoreNode::onInitialize]
      INITIALIZED, //!< Initialized [wait for ICoreNode::execute Action::CONFIGURE]
      CONFIGURING, //!< Configuring [call ICoreNode::onConfigure]
      CONFIGURED, //!< Configured [wait for ICoreNode::execute Action::PREPARE_HW]
      PREPARING_HW, //!< Preparing the HW [call ICoreNode::onPrepareHW]
      HW_READY, //!< HW is ready [wait for ICoreNode::execute Action::PREPARE_MW]
      PREPARING_MW, //!< Preparing the MW [call ICoreNode::onPrepareMW]
      MW_READY, //!< MW is ready [wait for ICoreNode::execute Action::PREPARE_MW]
      IDLE, //!< Node is idle [wait for ICoreNode::execute Action::START]
      STARTING, //!< Node is starting [call ICoreNode::onStart]
      LOOPING, //!< Node is looping [call ICoreNode::onLoop]
      STOPPING, //!< Node is stopping [call ICoreNode::onStop]
      FINALIZING, //!< Node is finalizing [call ICoreNode::onFinalize]
      ERROR, //!< Node is in an error state [call ICoreNode::onError]
      TEARING_DOWN //!< Node is being teared down
   };

   enum class Action {
      INITIALIZE, //!< Initialize
      CONFIGURE, //!< Configure
      PREPARE_HW, //!< Prepare the HW
      PREPARE_MW, //!< Prepare the MW
      START, //!< Start
      STOP, //!< Stop
      FINALIZE //!< Finalize
   };

   /*! \brief Create the node
    *
    */
   virtual bool
   setup() = 0;


   /*! \brief Tear down the node
    *
    */
   virtual bool
   teardown() = 0;


   /*! \brief Executes an action
    *
    */
   virtual bool
   execute(
      Action what
   ) = 0;


   /*! \brief Returns the status tha node is into
    *
    */
   virtual State
   state() const = 0;


   virtual ~ICoreNode() {}

protected:
   /*! \brief Initialize event
    *
    * \note Implementation must call onInitialize whenever the action is Action::INITIALIZE in a State::SET_UP state
    */
   virtual bool
   onInitialize() = 0;


   /*! \brief Configure event
    *
    * \note Implementation must call onConfigure whenever the action is Action::CONFIGURE in a State::INITIALIZED state
    */
   virtual bool
   onConfigure() = 0;


   /*! \brief HW preparation event
    *
    * \note Implementation must call onPrepareHW whenever the action is Action::PREPARE_HW in a State::CONFIGURED state
    */
   virtual bool
   onPrepareHW() = 0;


   /*! \brief MW preparation event
    *
    * \note Implementation must call onPrepareMW whenever the action is Action::PREPARE_MW in a State::HW_READY state
    */
   virtual bool
   onPrepareMW() = 0;


   /*! \brief Start event
    *
    * \note Implementation must call onStart whenever the action is Action::START in a State::IDLE state
    */
   virtual bool
   onStart() = 0;


   /*! \brief Loop event
    *
    * \note Implementation must call onLoop whenever node is in State::LOOPING state
    */
   virtual bool
   onLoop() = 0;


   /*! \brief Stop event
    *
    * \note Implementation must call onStop whenever the action is Action::STOP in a State::LOOPING state
    */
   virtual bool
   onStop() = 0;


   /*! \brief Error event
    *
    * \note Implementation must call onStop whenever the node is in State::ERROR state
    */
   virtual bool
   onError() = 0;


   /*! \brief Finalize event
    *
    * \note Implementation must call onFinalize whenever the action is Action::FINAALIZE in a State::IDLE state
    */

   virtual bool
   onFinalize() = 0;


protected:
   ICoreNode() :
      _currentState(State::NONE)
   {}

   State _currentState;
};

NAMESPACE_CORE_MW_END
