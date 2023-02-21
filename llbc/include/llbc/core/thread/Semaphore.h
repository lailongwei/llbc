// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

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
