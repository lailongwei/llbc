/**
 * @file    RWLock.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/03
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_RW_LOCK_H__
#define __LLBC_CORE_THREAD_RW_LOCK_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/SimpleLock.h"
#include "llbc/core/thread/ConditionVariable.h"

__LLBC_NS_BEGIN

/**
 * RWLock class encapsulation.
 */
class LLBC_EXPORT LLBC_RWLock
{
    /**
     * RWLock holder structure.
     */
    struct Holder
    {
        LLBC_SimpleLock lock;
        LLBC_ConditionVariable readCond;
        LLBC_ConditionVariable writeCond;
        int vaild;
        int r_active;
        int w_active;
        int r_wait;
        int w_wait;
    };
public:
    LLBC_RWLock();
    virtual ~LLBC_RWLock();

public:
    /**
     * Acquire read lock.
     */
    void ReadLock();

    /**
     * Try acquire read lock.
     */
    bool ReadTryLock();

    /**
     * Release read lock.
     */
    void ReadUnlock();

    /**
     * Acquire write lock.
     */
    void WriteLock();

    /**
     * Try acquire write lock.
     */
    bool WriteTryLock();

    /**
     * Release write lock.
     */
    void WriteUnlock();

private:
    Holder _holder;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_RW_LOCK_H__
