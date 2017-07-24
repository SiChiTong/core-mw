/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/CoreConfigurationManager.hpp>

NAMESPACE_CORE_MW_BEGIN

CoreConfigurationManager::CoreConfigurationManager()
{}

CoreConfigurationManager::~CoreConfigurationManager()
{}

void
CoreConfigurationManager::add(
    const CoreConfigurableBase& configurableObject
)
{
    _objects.link(configurableObject.link);
}

void
CoreConfigurationManager::dumpTo(
    uint8_t*    storage,
    std::size_t size
)
{
    std::size_t offset = 0;
    std::size_t cnt    = 0;

    for (const CoreConfigurableBase& object : _objects) {
        // For every object...
        if (object.dumpConfigurationTo(storage + sizeof(std::size_t), offset, size - sizeof(std::size_t))) {
            // If the conf has been dumped...
            cnt++; // Update the number of blocks
        }
    }

    memcpy(storage, &cnt, sizeof(std::size_t)); // Number of conf blocks
}

void
CoreConfigurationManager::setFrom(
    uint8_t*    storage,
    std::size_t size
)
{
    std::size_t offset = 0;
    std::size_t cnt    = 0;

    memcpy(&cnt, storage, sizeof(std::size_t)); // Number of conf blocks
    if(cnt != 0xFFFFFFFF) { // Erased flash?
        offset += sizeof(std::size_t);

        for (std::size_t i = 0; i < cnt; i++) {
            // For every block...
            for (CoreConfigurableBase& object : _objects) {
                // Try with every object...
                if (object.setConfigurationFrom(storage, offset, size - sizeof(std::size_t))) {
                    // If the block matches the object...
                    break; // Go on with the next block!
                }
            }
        }
    }
} // CoreConfigurationManager::setFrom

void
CoreConfigurationManager::loadFrom(
    CoreConfigurationStorage& storage
)
{
    if (storage.size() != 0) {
        setFrom(reinterpret_cast<uint8_t*>(storage.data()), storage.size());
    }
}

void
CoreConfigurationManager::saveTo(
    CoreConfigurationStorage& storage
)
{
    static_assert(sizeof(CoreConfigurationBase::Signature) == 4, "sizeof(CoreConfigurationBase::Signature) is not 4");

    bool success = true;

    if (storage.size() != 0) {
        success &= storage.beginWrite();

        std::size_t offset = 0;
        std::size_t cnt    = 0;

        offset += sizeof(std::size_t); // Number of conf blocks

        for (CoreConfigurableBase& object : _objects) {
            const uint16_t* tmp16 = nullptr;
            const uint32_t* tmp32 = nullptr;


            char tmp_buffer[NamingTraits < CoreConfigurableBase > ::MAX_LENGTH] CORE_MEMORY_ALIGNED;
            memset(tmp_buffer, 0, NamingTraits<CoreConfigurableBase>::MAX_LENGTH);
            strcpy(tmp_buffer, object.getKey());

            tmp16 = reinterpret_cast<const uint16_t*>(tmp_buffer); //NOTE: warning: cast increases required alignment of target type - It is OK by design

            for (std::size_t i = 0; i < NamingTraits<CoreConfigurableBase>::MAX_LENGTH; i += 2) {
                success &= storage.write16(offset + i, *tmp16++);
            }

            offset += NamingTraits<CoreConfigurableBase>::MAX_LENGTH;

            success &= storage.write32(offset, object.getConfigurationSignature());
            offset  += sizeof(CoreConfigurationBase::Signature);

            std::size_t dataSize = object.getConfigurationSize();

            tmp32 = reinterpret_cast<const uint32_t*>(&object.getConfigurationBase());

            for (std::size_t i = 0; i < dataSize; i += 4) {
                success &= storage.write32(offset + i, *tmp32++);
            }

            offset += dataSize;

            cnt++;
        }

        CORE_ASSERT(cnt != 0xFFFFFFFF); // Sorry, 4294967295 configurations are too many...

        success &= storage.write32(0, cnt);

        success &= storage.endWrite();
    }

    CORE_ASSERT(success);
} // CoreConfigurationManager::dumpTo

NAMESPACE_CORE_MW_END
