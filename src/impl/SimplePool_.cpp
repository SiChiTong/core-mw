/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/impl/SimplePool_.hpp>

NAMESPACE_CORE_MW_BEGIN


void*
SimplePool_::alloc_unsafe()
{
    if (headp != NULL) {
        void* blockp = headp;
        headp = headp->nextp;
        return blockp;
    }

    return NULL;
}

void
SimplePool_::free_unsafe(
    void* objp
)
{
    if (objp != NULL) {
        reinterpret_cast<Header*>(objp)->nextp = headp;
        headp = reinterpret_cast<Header*>(objp);
    }
}

void*
SimplePool_::alloc()
{
    core::os::SysLock::acquire();

    if (headp != NULL) {
        void* blockp = headp;
        headp = headp->nextp;
        core::os::SysLock::release();
        return blockp;
    } else {
        core::os::SysLock::release();
        return NULL;
    }
}

void
SimplePool_::free(
    void* objp
)
{
    if (objp != NULL) {
        core::os::SysLock::acquire();
        reinterpret_cast<Header*>(objp)->nextp = headp;
        headp = reinterpret_cast<Header*>(objp);
        core::os::SysLock::release();
    }
}

void
SimplePool_::grow(
    void*  arrayp,
    size_t length,
    size_t blocklen
)
{
    CORE_ASSERT(arrayp != NULL);
    CORE_ASSERT(length > 0);
    CORE_ASSERT(blocklen >= sizeof(void*));

    for (uint8_t* curp = reinterpret_cast<uint8_t*>(arrayp);
         length-- > 0; curp += blocklen) {
        free(curp);
    }
}

SimplePool_::SimplePool_()
    :
    headp(NULL)
{}


SimplePool_::SimplePool_(
    void*  arrayp,
    size_t length,
    size_t blocklen
)
    :
    headp(NULL)
{
    grow(arrayp, length, blocklen);
}

NAMESPACE_CORE_MW_END
