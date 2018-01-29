/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
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
protected:
    Item*  _arrayp;
    size_t _length;
    size_t _count;
    Item*  _headp;
    Item*  _tailp;

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
    if (_count < _length) {
        ++_count;
        *_tailp = item;

        if (++_tailp >= &_arrayp[_length]) {
            _tailp = &_arrayp[0];
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
    if (_count > 0) {
        --_count;
        item = *_headp;

        if (++_headp >= &_arrayp[_length]) {
            _headp = &_arrayp[0];
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
    if (_count > 0) {
        --_count;

        if (++_headp >= &_arrayp[_length]) {
            _headp = &_arrayp[0];
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
    return _length;
}

template <typename Item>
inline
size_t
ArrayQueue<Item>::get_count() const
{
    return _count;
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
    _arrayp(array),
    _length(length),
    _count(0),
    _headp(array),
    _tailp(array)
{
    CORE_ASSERT(array != nullptr);
    CORE_ASSERT(_length > 0);
}

NAMESPACE_CORE_MW_END
