/**
 * @file    ILock.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_ILOCK_H__
#define __LLBC_CORE_THREAD_ILOCK_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN
class LLBC_ConditionVariable;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief All lock interface class encapsulation.
 */
class LLBC_EXPORT LLBC_ILock
{
public:
    virtual ~LLBC_ILock() {  }

public:
    /**
     * Acquire lock.
     */
    virtual void Lock() = 0;

    /**
     * Try acquire lock.
     */
    virtual bool TryLock() = 0;

    /**
     * Release lock.
     */
    virtual void Unlock() = 0;

#if LLBC_TARGET_PLATFORM_NON_WIN32
private:
    friend class LLBC_ConditionVariable;
    virtual void *Handle() = 0;
#endif
};

__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_ILOCK_H__
