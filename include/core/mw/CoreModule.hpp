/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/CoreNodeManager.hpp>
#include <core/mw/CoreConfigurationManager.hpp>
#include <core/hw/UID.hpp>

NAMESPACE_CORE_MW_BEGIN

/*! \brief Base class for all Nova Core modules.
 *
 */
class CoreModule
{
public:
    using UID = uint32_t; // Alias for cleanliness

public:
    CoreModule();

    virtual ~CoreModule() {}

    /*! \ brief Module status led class.
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    class Led
    {
    public:
        void
        toggle();

        void
        write(
            unsigned on
        );
    };

    /*! \brief Reference to the module status led.
     *
     */
    static Led& led;

    /*! \brief Nodes running on the module.
     *
     */
    static
    CoreNodeManager&
    nodes();


    /*! \brief Configurations for configurable objects.
     *
     */
    static
    CoreConfigurationManager&
    configurations();


    /*! \brief Storage for configurations.
     *
     */
    static core::mw::CoreConfigurationStorage&
    configurationStorage();


    /*! \brief Module UID.
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    static const UID&
    uid();


    /*! \brief Module Name.
     *
     * Name of the module.
     *
     * The default implementations returns:
     * 1. if OVERRIDE_MODULE_ID is defined: OVERRIDE_MODULE_ID
     * 2. if the module is compiled with bootloader support: the name stored in the configuration block
     * 3. else the module type (e.g.: "io" for an "io" module)
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    static const char*
    name();


    /*! \brief Module CAN ID
     *
     * CAN ID of the module.
     *
     * The default implementations returns:
     * 1. if OVERRIDE_MODULE_ID is defined: OVERRIDE_MODULE_ID
     * 2. if the module is compiled with bootloader support: the ID stored in the configuration block
     * 3. else the last byte of the UID
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    static uint8_t
    canID();


    /*! \brief Initialize the module
     *
     * It initializes the OS.
     *
     * Derived modules can override it to initialize the HW and to set up the MW.
     */
    static bool
    initialize();


    /*! \brief Halts the module
     *
     * It stops the module forever.
     *
     * \warning No actions are taken on the HW.
     */
    static const void
    halt(
        const char* message //!< Debug message.
    );


    /*! \brief Reset the module.
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    static void
    reset();


    /*! \brief Keep the module alive.
     *
     * \warning If the module is compiled with bootloader support, this must be called at least once every 800ms.
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    static void
    keepAlive();


    /*! \brief Disable the bootloader.
     *
     * If the module is compiled with bootloader support, this disable the bootloader at the next reset.
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    static void
    disableBootloader();


    /*! \brief Disable the bootloader.
     *
     * If the module is compiled with bootloader support, this forces the bootloader at the next reset.
     *
     * \note This is HW specific, thus it must be implemented by the derived module.
     */
    static void
    enableBootloader();


private:
    static core::mw::CoreConfigurationStorage& _coreConfigurationStorage;
    static CoreNodeManager _nodes;
    static CoreConfigurationManager _configurations;
};

NAMESPACE_CORE_MW_END
