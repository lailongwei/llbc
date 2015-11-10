/**
 * @file    ConditionVariable.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_CONDITION_VARIABLE_H__
#define __LLBC_CORE_THREAD_CONDITION_VARIABLE_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN
class LLBC_ILock;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief CV class encapsulation.
 */
class LLBC_EXPORT LLBC_ConditionVariable
{
public:
    LLBC_ConditionVariable();
    ~LLBC_ConditionVariable();

public:
    /**
     * Notify one thread CV triggered.
     */
    void Notify();

    /**
     * Broadcast all waiters CV triggered.
     */
    void Broadcast();

    /**
     * Wait CV become usable.
     * @param[in] lock - condition lock, before call this method, this lock must in locked state.
     */
    void Wait(LLBC_ILock &lock);

    /**
     * Timed wait CV become usable.
     * @param[in] lock         - condition lock, before call this method, this lock must in locked state.
     * @param[in] milliSeconds - timed out parameter, in milliseconds.
     * @return int - return 0 if success, otherwise return -1.
     */
    int TimedWait(LLBC_ILock &lock, int milliSeconds);

private:
#if LLBC_TARGET_PLATFORM_WIN32
    /**
     * Private method.
     * @param[in] unblockAll - unblock all waiters flag.
     */
    void Notify(bool unblockAll);

    /**
     * Private method, after wait, will call it.
     * @param[in] lock      - lock object.
     */
    void AfterWait(LLBC_ILock &lock);
#endif // LLBC_TARGET_PLATFORM_WIN32

private:
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_cond_t m_handle;
#else // LLBC_TARGET_PLATFORM_WIN32
    struct 
    {
        long waitersBlocked;   // Number of threads blocked.
        long waitersGone;      // Number of threads timed out.
        long waitersToUnblock; // Number of threads to unblock.

        HANDLE blockQueue; // Queue up threads waiting for the condition to be become signalled.

        HANDLE blockLock;             // Semaphore that guards access to waiters blocked count/block queue.
        CRITICAL_SECTION unblockLock; // Mutex that guards access to waiters (to)unblock(ed) counts.
    } _cond;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_CONDITION_VARIABLE_H__
