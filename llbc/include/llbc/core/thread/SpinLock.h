/**
 * @file    SpinLock.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_SPIN_LOCK_H__
#define __LLBC_CORE_THREAD_SPIN_LOCK_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/ILock.h"

__LLBC_NS_BEGIN

/**
 * \brief Spin lock encapsulation.
 */
class LLBC_EXPORT LLBC_SpinLock : public LLBC_ILock
{
public:
    LLBC_SpinLock();
    virtual ~LLBC_SpinLock();

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
    void *Handle();
#endif

    LLBC_DISABLE_ASSIGNMENT(LLBC_SpinLock);

private:
    LLBC_SpinLockHandle _handle;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_SPIN_LOCK_H__
