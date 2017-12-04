/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>
#include <core/mw/ArrayQueue.hpp>
#include <core/os/Mutex.hpp>
#include <core/os/Condition.hpp>

NAMESPACE_CORE_MW_BEGIN

/*! \brief An ArrayQueue, but locked.
 *
 * \warning This is WIP - and so it is subject to changes
 */
template <typename Item>
class LockedArrayQueue:
    protected core::mw::ArrayQueue<Item>
{
public:
    LockedArrayQueue(
        Item*       storage,
        std::size_t length
    ) : core::mw::ArrayQueue<Item>(storage, length), _totalPushed(0), _totalPopped(0)
    {
        _mutex.initialize();
        _notFull.initialize();
        _notEmpty.initialize();
    }

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    void
    push(
        Item item
    )
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        _mutex.acquire_unsafe();

        while (!this->post_unsafe(item)) {
            _notFull.wait_unsafe();
        }

        _notEmpty.signal_unsafe();

        _totalPushed++;

        _mutex.release_unsafe();
    } // push

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    bool
    push_timeout(
        Item item,
		core::os::Time timeout
    )
    {
    	bool success = false;

        core::os::SysLock::ScopeFrom<CTX> lock;

        _mutex.acquire_unsafe();

        while (!this->post_unsafe(item)) {
        	 success = _notFull.wait_unsafe(timeout);
			if(!success) {
				break;
			}
        }

        if(success) {
        	_totalPushed++;
        	_notEmpty.signal_unsafe();
        }

        _mutex.release_unsafe();

        return success;
    } // push

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    bool
    try_push(
        Item item
    )
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        _mutex.acquire_unsafe();

        bool success = this->post_unsafe(item);

        if (success) {
            _totalPushed++;
            _notEmpty.signal_unsafe();
        }

        _mutex.release_unsafe();

        return success;
    } // push

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    void
    pop(
        Item& item
    )
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        _mutex.acquire_unsafe();

        while (!this->fetch_unsafe(item)) {
            _notEmpty.wait_unsafe();
        }

        _notFull.signal_unsafe();

        _totalPopped++;

        _mutex.release_unsafe();
    } // pop

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    bool
    pop_timeout(
        Item& item,
		core::os::Time timeout
    )
    {
    	bool success = false;

        core::os::SysLock::ScopeFrom<CTX> lock;

        _mutex.acquire_unsafe();

        while (!this->fetch_unsafe(item)) {
            success = _notEmpty.wait_unsafe(timeout);
            if(!success) {
            	break;
            }
        }

        if (success) {
            _totalPopped++;
            _notFull.signal_unsafe();
            _mutex.release_unsafe();
        }

        return success;
    } // pop_timeout

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    bool
    try_pop(
        Item& item
    )
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        _mutex.acquire_unsafe();

        bool success = this->fetch_unsafe(item);

        if (success) {
            _totalPopped++;
            _notFull.signal_unsafe();
        }

        _mutex.release_unsafe();

        return success;
    } // pop

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    inline
    size_t
    count()
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        return this->get_count();
    }

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    inline
    size_t
    length()
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        return this->get_length();
    }

private:
    core::os::Mutex     _mutex;
    core::os::Condition _notFull;
    core::os::Condition _notEmpty;
    std::size_t         _totalPushed;
    std::size_t         _totalPopped;
};

NAMESPACE_CORE_MW_END
