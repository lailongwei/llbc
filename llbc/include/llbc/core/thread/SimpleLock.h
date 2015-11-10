/**
 * @file    SimpleLock.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_SIMPLE_LOCK_H__
#define __LLBC_CORE_THREAD_SIMPLE_LOCK_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/ILock.h"

__LLBC_NS_BEGIN

/**
 * \brief Simple lock encapsulation.
 */
class LLBC_EXPORT LLBC_SimpleLock : public LLBC_ILock
{
public:
    LLBC_SimpleLock();
    virtual ~LLBC_SimpleLock();

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

private:
#if LLBC_TARGET_PLATFORM_NON_WIN32
    friend class LLBC_ConditionVariable;
    virtual void *Handle();
#endif

    LLBC_DISABLE_ASSIGNMENT(LLBC_SimpleLock);

private:
#if LLBC_TARGET_PLATFORM_WIN32
    bool _locked;
#endif

    LLBC_MutexHandle _handle;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_SIMPLE_LOCK_H__
