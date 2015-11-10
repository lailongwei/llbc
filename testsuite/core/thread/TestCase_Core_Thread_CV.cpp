/**
 * @file    TestCase_Core_Thread_CV.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

#include "core/thread/TestCase_Core_Thread_CV.h"

static struct 
{
    LLBC_SimpleLock lock;
    LLBC_ConditionVariable cv;
    int value;
} __g_testData;

static LLBC_SimpleLock __g_outLock;
static const int __g_waiterThreadCount = 5;

static int WaiterThreadProc(void *arg)
{
    const int threadIndex = static_cast<int>(reinterpret_cast<long>(arg));

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
            if(__g_testData.cv.TimedWait(__g_testData.lock, 510) != LLBC_RTN_OK)
            {
                if(LLBC_GetLastError() != LLBC_ERROR_TIMEOUT)
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

    return 0;
}

static int SignalerThreadProc(void *arg)
{
    LLBC_Sleep(1000);

    __g_outLock.Lock();
    std::cout <<"I'm signaler thread." <<std::endl;
    __g_outLock.Unlock();

    for(int i = 1; i <= 100; i ++)
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

    return 0;
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
    for(int i = 0; i < __g_waiterThreadCount; i ++)
    {
        LLBC_CreateThread(&waiters[i], &WaiterThreadProc, reinterpret_cast<void *>(i));
    }

    // Create signaler.
    LLBC_NativeThreadHandle signaler = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    LLBC_CreateThread(&signaler, &SignalerThreadProc, NULL);

    // Join signaler.
    LLBC_JoinThread(signaler);

    // Cancel and join signaler.
    for(int i = 0; i < __g_waiterThreadCount; i ++)
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
