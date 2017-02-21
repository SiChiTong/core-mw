/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>

NAMESPACE_CORE_MW_BEGIN


template <typename Item>
class ArrayQueue:
    private core::Uncopyable
{
private:
    Item*  arrayp;
    size_t length;
    size_t count;
    Item*  headp;
    Item*  tailp;

public:
    bool
    post_unsafe(
        Item item
    );

    bool
    fetch_unsafe(
        Item& item
    );

    bool
    skip_unsafe();

    size_t
    get_length() const;

    size_t
    get_count() const;

    bool
    post(
        Item item
    );

    bool
    fetch(
        Item& item
    );

    bool
    skip();


public:
    ArrayQueue(
        Item   array[],
        size_t length
    );
};


// TODO: Low-level implementation


template <typename Item>
inline
bool
ArrayQueue<Item>::post_unsafe(
    Item item
)
{
    if (count < length) {
        ++count;
        *tailp = item;

        if (++tailp >= &arrayp[length]) {
            tailp = &arrayp[0];
        }

        return true;
    }

    return false;
}

template <typename Item>
inline
bool
ArrayQueue<Item>::fetch_unsafe(
    Item& item
)
{
    if (count > 0) {
        --count;
        item = *headp;

        if (++headp >= &arrayp[length]) {
            headp = &arrayp[0];
        }

        return true;
    }

    return false;
}

template <typename Item>
inline
bool
ArrayQueue<Item>::skip_unsafe()
{
    if (count > 0) {
        --count;

        if (++headp >= &arrayp[length]) {
            headp = &arrayp[0];
        }

        return true;
    }

    return false;
}

template <typename Item>
inline
size_t
ArrayQueue<Item>::get_length() const
{
    return length;
}

template <typename Item>
inline
size_t
ArrayQueue<Item>::get_count() const
{
    return count;
}

template <typename Item>
inline
bool
ArrayQueue<Item>::post(
    Item item
)
{
    core::os::SysLock::acquire();
    bool success = post_unsafe(item);
    core::os::SysLock::release();

    return success;
}

template <typename Item>
inline
bool
ArrayQueue<Item>::fetch(
    Item& item
)
{
    core::os::SysLock::acquire();
    bool success = fetch_unsafe(item);
    core::os::SysLock::release();

    return success;
}

template <typename Item>
inline
bool
ArrayQueue<Item>::skip()
{
    core::os::SysLock::acquire();
    bool success = skip_unsafe();
    core::os::SysLock::release();

    return success;
}

template <typename Item>
inline
ArrayQueue<Item>::ArrayQueue(
    Item   array[],
    size_t length
)
    :
    arrayp(array),
    length(length),
    count(0),
    headp(array),
    tailp(array)
{
    CORE_ASSERT(array != nullptr);
    CORE_ASSERT(length > 0);
}

NAMESPACE_CORE_MW_END
