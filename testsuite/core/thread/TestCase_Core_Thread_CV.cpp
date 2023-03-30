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


#include "core/thread/TestCase_Core_Thread_CV.h"

static struct 
{
    LLBC_SimpleLock lock;
    LLBC_ConditionVariable cv;
    int value;
} __g_testData;

static LLBC_SimpleLock __g_outLock;
static const long __g_waiterThreadCount = 5;

static void WaiterThreadProc(void *arg)
{
    long threadIndex;
    memcpy(&threadIndex, &arg, sizeof(long));

    __g_outLock.Lock();
    std::cout <<"I'm waiter thread " <<threadIndex <<std::endl;
    __g_outLock.Unlock();

    while(true)
    {
        __g_outLock.Lock();
        std::cout <<"thread [" <<threadIndex <<"] begin wait" <<std::endl;
        __g_outLock.Unlock();

        __g_testData.lock.Lock();
        if(threadIndex % 4 != 0)
        {
            __g_testData.cv.Wait(__g_testData.lock);
        }
        else
        {
            if(__g_testData.cv.TimedWait(__g_testData.lock, 510) != LLBC_OK)
            {
                if(LLBC_GetLastError() != LLBC_ERROR_TIMEOUTED)
                {
                    __g_outLock.Lock();
                    std::cout <<"thread [" <<threadIndex 
                        <<"] timedwait failed, err: " <<LLBC_FormatLastError() <<std::endl;
                    __g_outLock.Unlock();
                }

                __g_testData.lock.Unlock();

                continue;
            }
        }

        __g_outLock.Lock();
        const int value = __g_testData.value;
        std::cout <<"thread [" <<threadIndex <<"] weakup, get value: " <<value <<std::endl;
        __g_outLock.Unlock();

        __g_testData.lock.Unlock();

        if(value == 0)
        {
            __g_outLock.Lock();
            std::cout <<"thread [" <<threadIndex <<"] exit." <<std::endl;
            __g_outLock.Unlock();

            break;
        }
    }
}

static void SignalerThreadProc(void *arg)
{
    LLBC_Sleep(1000);

    __g_outLock.Lock();
    std::cout <<"I'm signaler thread." <<std::endl;
    __g_outLock.Unlock();

    for(int i = 1; i <= 100; ++i)
    {
        LLBC_Sleep(500);

        __g_outLock.Lock();
        if(i % 4 != 0)
        {
            std::cout <<"Signal CV" <<std::endl;
        }
        else
        {
            std::cout <<"Broadcast CV" <<std::endl;
        }
        __g_outLock.Unlock();

        __g_testData.lock.Lock();
        __g_testData.value = i;
        if(i % 4 != 0)
        {
            __g_testData.cv.Notify();
        }
        else
        {
            __g_testData.cv.Broadcast();
        }
        __g_testData.lock.Unlock();
    }

    __g_outLock.Lock();
    std::cout <<"Broadcast cv" <<std::endl;
    __g_outLock.Unlock();

    __g_testData.lock.Lock();
    __g_testData.value = 0;
    __g_testData.cv.Broadcast();
    __g_testData.lock.Unlock();

    LLBC_Sleep(1000);

    __g_outLock.Lock();
    std::cout <<"Signaler thread exit." <<std::endl;
    __g_outLock.Unlock();
}

TestCase_Core_Thread_CV::TestCase_Core_Thread_CV()
{
}

TestCase_Core_Thread_CV::~TestCase_Core_Thread_CV()
{
}

int TestCase_Core_Thread_CV::Run(int argc, char *argv[])
{
    std::cout <<"core/thread/cv test: " <<std::endl;

    // Create waiters.
    LLBC_NativeThreadHandle waiters[__g_waiterThreadCount] = {LLBC_INVALID_NATIVE_THREAD_HANDLE};
    for(long i = 0; i < __g_waiterThreadCount; ++i)
    {
        void *threadArg = nullptr;
        memcpy(&threadArg, &i, sizeof(long));
        LLBC_CreateThread(&WaiterThreadProc, threadArg, &waiters[i]);
    }

    // Create signaler.
    LLBC_NativeThreadHandle signaler = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    LLBC_CreateThread(&SignalerThreadProc, nullptr, &signaler);

    // Join signaler.
    LLBC_JoinThread(signaler);

    // Cancel and join signaler.
    for(long i = 0; i < __g_waiterThreadCount; ++i)
    {
        LLBC_CancelThread(waiters[i]);
        LLBC_JoinThread(waiters[i]);
    }

    __g_outLock.Lock();
    std::cout <<"Press any key to continue ... ..." <<std::endl;
    __g_outLock.Unlock();
    getchar();

    return 0;
}
