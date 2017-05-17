/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

#include <core/mw/StaticList.hpp>
#include <core/mw/CoreConfiguration.hpp>

#include <type_traits>

NAMESPACE_CORE_MW_BEGIN
class CoreConfigurationStorage
{
public:
    // begin read
    // end read
    virtual void*
    data()
    {
        return nullptr;
    }

    virtual std::size_t
    size()
    {
        return 0;
    }

    virtual bool
    beginWrite()
    {
        return false;
    }

    virtual bool
    write16(
        std::size_t address,
        uint16_t    data
    )
    {
        return false;
    }

    virtual bool
    write32(
        std::size_t address,
        uint32_t    data
    )
    {
        return false;
    }

    virtual bool
    endWrite()
    {
        return false;
    }
};

class CoreConfigurationManager
{
public:
    CoreConfigurationManager();
    virtual
    ~CoreConfigurationManager();

    void
    add(
        const CoreConfigurableBase& configurableObject
    );


    template <typename CO, typename C>
    void
    add(
        CO& configurableObject,
        C&  configuration
    );

    void
    setFrom(
        uint8_t*    storage,
        std::size_t size
    );

    void
    dumpTo(
        uint8_t*    storage,
        std::size_t size
    );

    void
    loadFrom(
        CoreConfigurationStorage& storage
    );

    void
    saveTo(
        CoreConfigurationStorage& storage
    );


private:
    core::mw::StaticList<CoreConfigurableBase> _objects;
};


template <typename CO, typename C>
void
CoreConfigurationManager::add(
    CO& configurableObject,
    C&  configuration
)
{
    static_assert(std::is_base_of<CoreConfigurableBase, CO>::value, "configurableObject does not inherit from CoreConfigurableBase");
    static_assert(std::is_base_of<CoreConfigurationBase, C>::value, "configuration does not inherit from CoreConfigurationBase");
    static_assert(std::is_same<typename CO::ConfigurationType, C>::value, "configuration is not compatible with configurableObject");

    configurableObject.setConfiguration(configuration);

    add(configurableObject);
}

NAMESPACE_CORE_MW_END
