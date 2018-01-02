/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/impl/SimplePool_.hpp>

NAMESPACE_CORE_MW_BEGIN


template <typename Item>
class SimplePool:
    private core::Uncopyable
{
private:
    SimplePool_ impl;

public:
    Item*
    alloc_unsafe();

    void
    free_unsafe(
        Item* objp
    );

    Item*
    alloc();

    void
    free(
        Item* objp
    );

    void
    grow(
        Item   array[],
        size_t length
    );


public:
    SimplePool();
    SimplePool(
        Item   array[],
        size_t length
    );
};


template <typename Item>
inline
Item*
SimplePool<Item>::alloc_unsafe()
{
    return impl.alloc_unsafe();
}

template <typename Item>
inline
void
SimplePool<Item>::free_unsafe(
    Item* objp
)
{
    impl.free_unsafe(objp);
}

template <typename Item>
inline
Item*
SimplePool<Item>::alloc()
{
    return impl.alloc();
}

template <typename Item>
inline
void
SimplePool<Item>::free(
    Item* objp
)
{
    impl.free(objp);
}

template <typename Item>
inline
void
SimplePool<Item>::grow(
    Item   array[],
    size_t length
)
{
    impl.grow(reinterpret_cast<void*>(array), length, sizeof(Item));
}

NAMESPACE_CORE_MW_END
