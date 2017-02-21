/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/namespace.hpp>
#include <core/mw/impl/StaticQueue_.hpp>

NAMESPACE_CORE_MW_BEGIN


const StaticQueue_::Link*
StaticQueue_::get_head_unsafe() const
{
    return headp;
}

const StaticQueue_::Link*
StaticQueue_::get_tail_unsafe() const
{
    return tailp;
}

void
StaticQueue_::post_unsafe(
    Link& link
)
{
    CORE_ASSERT((headp != nullptr) == (tailp != nullptr));
    CORE_ASSERT(link.nextp == nullptr);

    if (tailp != nullptr) {
        CORE_ASSERT(tailp->nextp == nullptr);
        tailp->nextp = &link;
        tailp = &link;
    } else {
        headp = tailp = &link;
    }
}

bool
StaticQueue_::peek_unsafe(
    const Link*& linkp
) const
{
    CORE_ASSERT((headp != nullptr) == (tailp != nullptr));

    if (headp != nullptr) {
        linkp = headp;
        return true;
    }

    return false;
}

bool
StaticQueue_::fetch_unsafe(
    Link& link
)
{
    CORE_ASSERT((headp != nullptr) == (tailp != nullptr));

    if (headp != nullptr) {
        register Link* nextp = headp->nextp;
        headp->nextp = nullptr;
        link  = *headp;
        headp = nextp;

        if (headp == nullptr) {
            tailp = nullptr;
        }

        return true;
    }

    return false;
} // StaticQueue_::fetch_unsafe

bool
StaticQueue_::skip_unsafe()
{
    CORE_ASSERT((headp != nullptr) == (tailp != nullptr));

    if (headp != nullptr) {
        register Link* nextp = headp->nextp;
        headp->nextp = nullptr;
        headp = nextp;

        if (headp == nullptr) {
            tailp = nullptr;
        }

        return true;
    }

    return false;
} // StaticQueue_::skip_unsafe

const StaticQueue_::Link*
StaticQueue_::get_head() const
{
    core::os::SysLock::acquire();
    const Link* headp = get_head_unsafe();
    core::os::SysLock::release();

    return headp;
}

const StaticQueue_::Link*
StaticQueue_::get_tail() const
{
    core::os::SysLock::acquire();
    const Link* tailp = get_tail_unsafe();
    core::os::SysLock::release();

    return tailp;
}

bool
StaticQueue_::is_empty() const
{
    core::os::SysLock::acquire();
    bool isempty = is_empty_unsafe();
    core::os::SysLock::release();

    return isempty;
}

void
StaticQueue_::post(
    Link& link
)
{
    core::os::SysLock::acquire();

    post_unsafe(link);
    core::os::SysLock::release();
}

bool
StaticQueue_::peek(
    const Link*& linkp
) const
{
    core::os::SysLock::acquire();
    bool success = peek_unsafe(linkp);
    core::os::SysLock::release();

    return success;
}

bool
StaticQueue_::fetch(
    Link& link
)
{
    core::os::SysLock::acquire();
    bool success = fetch_unsafe(link);
    core::os::SysLock::release();

    return success;
}

bool
StaticQueue_::skip()
{
    core::os::SysLock::acquire();
    bool success = skip_unsafe();
    core::os::SysLock::release();

    return success;
}

StaticQueue_::StaticQueue_()
    :
    headp(nullptr),
    tailp(nullptr)
{}


NAMESPACE_CORE_MW_END
