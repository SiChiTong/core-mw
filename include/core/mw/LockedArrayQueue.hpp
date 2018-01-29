/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
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

#ifndef LOCKED_ARRAY_QUEUE_STATS
#define LOCKED_ARRAY_QUEUE_STATS 0
#endif

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
#if LOCKED_ARRAY_QUEUE_STATS
	, _min_level(0), _max_level(0)
#endif
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

#if LOCKED_ARRAY_QUEUE_STATS
        if(this->_count > _max_level) {
        	_max_level = this->_count;
        }
#endif

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

#if LOCKED_ARRAY_QUEUE_STATS
        if(this->_count > _max_level) {
        	_max_level = this->_count;
        }
#endif

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

#if LOCKED_ARRAY_QUEUE_STATS
        if(this->_count > _max_level) {
        	_max_level = this->_count;
        }
#endif

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

#if LOCKED_ARRAY_QUEUE_STATS
        if(this->_count < _min_level) {
        	_min_level = this->_count;
        }
#endif

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

#if LOCKED_ARRAY_QUEUE_STATS
        if(this->_count < _min_level) {
        	_min_level = this->_count;
        }
#endif

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

#if LOCKED_ARRAY_QUEUE_STATS
        if(this->_count < _min_level) {
        	_min_level = this->_count;
        }
#endif

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

#if LOCKED_ARRAY_QUEUE_STATS
    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    inline
    void
    reset_stats()
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        _max_level = this->_count;
        _min_level = this->_count;
    }

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    inline
    size_t
    max_level()
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        return _max_level;
    }

    template <core::os::CallingContext CTX = core::os::CallingContext::NORMAL>
    inline
    size_t
    min_level()
    {
        core::os::SysLock::ScopeFrom<CTX> lock;

        return _min_level;
    }
#endif
private:
    core::os::Mutex     _mutex;
    core::os::Condition _notFull;
    core::os::Condition _notEmpty;
    std::size_t         _totalPushed;
    std::size_t         _totalPopped;
#if LOCKED_ARRAY_QUEUE_STATS
    std::size_t         _min_level;
    std::size_t         _max_level;
#endif
};

NAMESPACE_CORE_MW_END
