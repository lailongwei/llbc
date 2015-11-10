/**
 * @file    Semaphore.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_SEMAPHORE_H__
#define __LLBC_CORE_THREAD_SEMAPHORE_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief Semaphore class encapsulation.
 */
class LLBC_EXPORT LLBC_Semaphore
{
public:
    explicit LLBC_Semaphore(int initVal = 0);
    ~LLBC_Semaphore();

public:
    /**
     * Wait the semaphore become signaled.
     */
    void Wait();

    /**
     * Try wait the semaphore become signaled.
     * @return bool - return true if try wait successed, otherwise return false.
     */
    bool TryWait();

    /**
     * Timed wait the semaphore become signaled.
     * @param[in] milliSeconds - specifies the time-out interval, in milliseconds.
     */
    bool TimedWait(int milliSeconds);

    /**
     * Post signal.
     * @param[in] count - post count.
     */
    void Post(int count = 1);

    LLBC_DISABLE_ASSIGNMENT(LLBC_Semaphore);

private:
#if LLBC_TARGET_PLATFORM_LINUX || LLBC_TARGET_PLATFORM_ANDROID
    sem_t _sem;
#elif LLBC_TARGET_PLATFORM_MAC || LLBC_TARGET_PLATFORM_IPHONE
    sem_t *_sem;
#else
    HANDLE _sem;
#endif
};

__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_SEMAPHORE_H__
