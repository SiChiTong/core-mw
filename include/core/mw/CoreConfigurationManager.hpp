/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>

#include <core/mw/StaticList.hpp>
#include <core/mw/CoreConfiguration.hpp>

NAMESPACE_CORE_MW_BEGIN
class CoreConfigurationStorage
{
public:
    virtual void*
    data() = 0;

    virtual std::size_t
    size() = 0;

    virtual bool
    beginWrite() {
        return false;
    }

    virtual bool
    write16(std::size_t address, uint16_t data) {
        return false;
    }

    virtual bool
    write32(std::size_t address, uint32_t data) {
        return false;
    }

    virtual bool
    endWrite() {
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

    void
    dumpTo(
        uint8_t*    storage,
        std::size_t size
    );

    void
    setFrom(
        uint8_t*    storage,
        std::size_t size
    );

    void
    setFrom(
        CoreConfigurationStorage& storage
    );

    void
    dumpTo(
        CoreConfigurationStorage& storage
    );

private:
    core::mw::StaticList<CoreConfigurableBase> _objects;
};

NAMESPACE_CORE_MW_END
