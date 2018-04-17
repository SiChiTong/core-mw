/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/CoreConfiguration.hpp>

NAMESPACE_CORE_MW_BEGIN

const CoreConfigurationMap::FieldMetadata
CoreConfigurationMap::getField(
    CoreConfigurationMap::Key key
) const
{
    // Returning a copy of the field... Not a reference (that would be to a local)
    for (FieldMetadata f : * this) {
        if (key == f.key) {
            return f;
        }
    }

    CORE_ASSERT(!"field does not exist");
    return {};
}

const std::size_t
CoreConfigurationMap::getFieldIndex(
    CoreConfigurationMap::Key key
) const
{
    std::size_t i = 0;

    for (FieldMetadata f : * this) {
        if (key == f.key) {
            return i;
        }

        i++;
    }

    return i; // this will be gt the last index. It will fire an assert in getField
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void
CoreConfigurationStatic_::set(
    CoreConfigurationBase*                     obj,
    const CoreConfigurationMap::FieldMetadata& field,
    const void*                                x
)
{
    std::size_t        s1 = field.size;
    core::CoreType t1 = field.type;

    std::size_t len = s1 * core::CoreTypeUtils::coreTypeSize(t1);

    const uint8_t* src = reinterpret_cast<const uint8_t*>(x);
    uint8_t*       dst = reinterpret_cast<uint8_t*>(obj + field.offset);

    while (len--) {
        *dst++ = *src++;
    }
}

void
CoreConfigurationStatic_::set(
    CoreConfigurationBase*                     obj,
    const CoreConfigurationMap::FieldMetadata& field,
    const char*                                x
)
{
    std::size_t s1 = field.size;
    std::size_t s2 = strlen(x);

    core::CoreType t1 = field.type;
    core::CoreType t2 = core::CoreType::CHAR;

    CORE_ASSERT(s1 >= s2 && t1 == t2);  // make sure we are doing something meaningful...

    const char* src = reinterpret_cast<const char*>(x);
    char*       dst = reinterpret_cast<char*>(obj + field.offset);

    std::size_t i = 0;

    while (i < s2) {
        // copy
        *dst++ = *src++;
        i++;
    }

    while (i < s1) {
        // pad
        *dst = 0;
        i++;
    }
} // set

//GET
void
CoreConfigurationStatic_::get(
    const CoreConfigurationBase*               obj,
    const CoreConfigurationMap::FieldMetadata& field,
    void*                                      x
)
{
    std::size_t        s1 = field.size;
    core::CoreType t1 = field.type;

    std::size_t len = s1 * core::CoreTypeUtils::coreTypeSize(t1);

    const uint8_t* src = reinterpret_cast<const uint8_t*>(obj + field.offset);
    uint8_t*       dst = reinterpret_cast<uint8_t*>(x);

    while (len--) {
        *dst++ = *src++;
    }
}

void
CoreConfigurationStatic_::get(
    const CoreConfigurationBase*               obj,
    const CoreConfigurationMap::FieldMetadata& field,
    char*                                      x
)
{
    std::size_t s1 = field.size;
    std::size_t s2 = strlen(x);

    core::CoreType t1 = field.type;
    core::CoreType t2 = core::CoreType::CHAR;

    CORE_ASSERT(s1 >= s2 && t1 == t2);    // make sure we are doing something meaningful...

    const char* src = reinterpret_cast<const char*>(obj + field.offset);
    char*       dst = reinterpret_cast<char*>(x);

    std::size_t i = 0;

    while (i < s2) {
        // copy
        *dst++ = *src++;
        i++;
    }

    while (i < s1) {
        // pad
        *dst = 0;
        i++;
    }
} // get

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


bool
CoreConfigurableBase::setConfigurationFrom(
    const void*  storage,
    std::size_t& offset,
    std::size_t  size
)
{
	CORE_ASSERT(_key != nullptr);

    std::size_t tmpOffset = offset;

    char name[NamingTraits < CoreConfigurableBase > ::MAX_LENGTH];

    memcpy(name, reinterpret_cast<const char*>((std::size_t)storage + tmpOffset), NamingTraits<CoreConfigurableBase>::MAX_LENGTH);

    if (strncmp(_key, name, NamingTraits<CoreConfigurableBase>::MAX_LENGTH) != 0) {
        return false;
    }

    tmpOffset += NamingTraits<CoreConfigurableBase>::MAX_LENGTH;

    CORE_ASSERT(size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize());

    CoreConfigurationBase::Signature signature;
    memcpy(&signature, reinterpret_cast<const void*>((std::size_t)storage + tmpOffset), sizeof(CoreConfigurationBase::Signature));

    if (signature != getConfigurationSignature()) {
        return false;
    }

    tmpOffset += sizeof(CoreConfigurationBase::Signature);

    setConfigurationBase(*reinterpret_cast<const CoreConfigurationBase*>((std::size_t)storage + tmpOffset));

    std::size_t dataSize = getConfigurationSize();

    tmpOffset += dataSize;

    if ((dataSize % 4) != 0) {
        tmpOffset += 4 - (dataSize % 4);
    }

    offset = tmpOffset;

    return true;
} // setConfigurationFrom

bool
CoreConfigurableBase::overrideConfigurationFrom(
    const void*  storage,
    std::size_t& offset,
    std::size_t  size
)
{
	CORE_ASSERT(_key != nullptr);

    std::size_t tmpOffset = offset;

    char name[NamingTraits < CoreConfigurableBase > ::MAX_LENGTH];

    memcpy(name, reinterpret_cast<const char*>((std::size_t)storage + tmpOffset), NamingTraits<CoreConfigurableBase>::MAX_LENGTH);

    if (strncmp(_key, name, NamingTraits<CoreConfigurableBase>::MAX_LENGTH) != 0) {
        return false;
    }

    tmpOffset += NamingTraits<CoreConfigurableBase>::MAX_LENGTH;

    CORE_ASSERT(size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize());

    CoreConfigurationBase::Signature signature;
    memcpy(&signature, reinterpret_cast<const void*>((std::size_t)storage + tmpOffset), sizeof(CoreConfigurationBase::Signature));

    if (signature != getConfigurationSignature()) {
        return false;
    }

    tmpOffset += sizeof(CoreConfigurationBase::Signature);

    overrideConfiguration();

    std::size_t dataSize = getConfigurationSize();

    memcpy(&getOverridingConfigurationBase(), reinterpret_cast<const void*>((std::size_t)storage + tmpOffset), dataSize);

    tmpOffset += dataSize;

    if ((dataSize % 4) != 0) {
        tmpOffset += 4 - (dataSize % 4);
    }

    offset = tmpOffset;

    return true;
} // overrideConfigurationFrom

bool
CoreConfigurableBase::dumpConfigurationTo(
    uint8_t*     storage,
    std::size_t& offset,
    std::size_t  size
) const
{
	CORE_ASSERT(_key != nullptr);
    CORE_ASSERT(size >= offset + NamingTraits<Node>::MAX_LENGTH + sizeof(CoreConfigurationBase::Signature) + getConfigurationSize());

    memcpy(storage + offset, _key, NamingTraits<Node>::MAX_LENGTH);
    offset += NamingTraits<Node>::MAX_LENGTH;

    CoreConfigurationBase::Signature signature = getConfigurationSignature();

    memcpy(storage + offset, &signature, sizeof(CoreConfigurationBase::Signature));
    offset += sizeof(CoreConfigurationBase::Signature);

    const CoreConfigurationBase* data = static_cast<const CoreConfigurationBase*>(_configuration);
    std::size_t dataSize = getConfigurationSize();

    memcpy(storage + offset, data, dataSize);

    offset += dataSize;

    if ((dataSize % 4) != 0) {
        offset += 4 - (dataSize % 4);
    }

    return true;
} // dumpTo

NAMESPACE_CORE_MW_END
