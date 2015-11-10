/**
 * @file    RecursiveLock.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_RECURSIVE_LOCK_H__
#define __LLBC_CORE_THREAD_RECURSIVE_LOCK_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/ILock.h"

__LLBC_NS_BEGIN

/**
 * \brief Recursive lock encapsulation.
 */
class LLBC_EXPORT LLBC_RecursiveLock : public LLBC_ILock
{
public:
    LLBC_RecursiveLock();
    virtual ~LLBC_RecursiveLock();

public:
    /**
     * Acquire lock.
     */
    virtual void Lock();

    /**
     * Try acquire lock.
     */
    virtual bool TryLock();

    /**
     * Release lock.
     */
    virtual void Unlock();

#if LLBC_TARGET_PLATFORM_NON_WIN32
private:
    friend class LLBC_ConditionVariable;
    virtual void *Handle();
#endif

    LLBC_DISABLE_ASSIGNMENT(LLBC_RecursiveLock);

private:
    LLBC_MutexHandle _handle;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_RECURSIVE_LOCK_H__
