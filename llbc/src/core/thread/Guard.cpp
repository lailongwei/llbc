/**
 * @file    Guard.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/ILock.h"

#include "llbc/core/thread/Guard.h"

__LLBC_NS_BEGIN

LLBC_FORCE_INLINE LLBC_LockGuard::LLBC_LockGuard(LLBC_ILock &lock, bool reverse)
: _lock(lock)
, _reverse(reverse)
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_LockGuard: locker[%p] will %s", &lock, (_reverse ? "unlock" : "lock"));
#endif // Guard debug

    if (!_reverse)
        _lock.Lock();
    else
        _lock.Unlock();

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_LockGuard: locker[%p] %s", &lock, (_reverse ? "unlocked" : "locked"));
#endif // Guard debug
}

LLBC_FORCE_INLINE LLBC_LockGuard::~LLBC_LockGuard()
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_LockGuard: locker[%p] will %s", &_lock, (_reverse ? "lock" : "unlock"));
#endif // Guard debug

    if (!_reverse)
        _lock.Unlock();
    else
        _lock.Lock();

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_LockGuard: locker[%p] %s", &_lock, (_reverse ? "locked" : "unlocked"));
#endif // Guard debug
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
