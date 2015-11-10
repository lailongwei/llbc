/**
 * @file    TestCase_Core_Thread_Semaphore.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

#include "core/thread/TestCase_Core_Thread_Semaphore.h"

static const int __g_waitersCount = 5;

struct __LLBC_SemTestType
{
    int value;
    LLBC_Semaphore sem;
};
static __LLBC_SemTestType *__g_testVal;

static LLBC_SimpleLock __g_outLock;

static int WaiterThreadProc(void *arg)
{
    int threadIndex = (int)((long)(arg));
    __g_testVal->sem.Wait();
    __g_outLock.Lock();
    std::cout <<"waiter: " <<threadIndex <<" weakup[Wait], get value: " <<__g_testVal->value <<std::endl;
    __g_outLock.Unlock();

    while(!__g_testVal->sem.TryWait())
    {
        LLBC_Sleep(0);
    }
    __g_outLock.Lock();
    std::cout <<"waiter: " <<threadIndex <<" weakup[TryWait], get value: " <<__g_testVal->value <<std::endl;
    __g_outLock.Unlock();

    while(!__g_testVal->sem.TimedWait(500))
    {
        __g_outLock.Lock();
        std::cout <<"waiter: " <<threadIndex 
            <<" wait timeout[TimedWait], get value: " <<__g_testVal->value <<std::endl;
        __g_outLock.Unlock();
    }
    __g_outLock.Lock();
    std::cout <<"waiter: " <<threadIndex <<" weakup[TimedWait], get value: " <<__g_testVal->value <<std::endl;
    __g_outLock.Unlock();

    __g_outLock.Lock();
    std::cout <<"waiter " <<threadIndex <<" exit" <<std::endl;
    __g_outLock.Unlock();

    return 0;
}

static int SignalerThreadProc(void *arg)
{
    LLBC_Sleep(1000);

    __g_outLock.Lock();
    std::cout <<"Signaler: send signal[1]" <<std::endl;
    __g_testVal->sem.Post(1);
    __g_outLock.Unlock();

    LLBC_Sleep(1000);
    __g_outLock.Lock();
    std::cout <<"Signaler: send signal[2]" <<std::endl;
    __g_testVal->sem.Post(2);
    __g_outLock.Unlock();

    LLBC_Sleep(2000);
    __g_outLock.Lock();
    std::cout <<"Signaler: send signal[3]" <<std::endl;
    __g_testVal->sem.Post(3);
    __g_outLock.Unlock();

    LLBC_Sleep(5000);
    __g_outLock.Lock();
    std::cout <<"Signaler: send signal[30]" <<std::endl;
    __g_testVal->sem.Post(30);
    __g_outLock.Unlock();

    LLBC_Sleep(10000);
    __g_outLock.Lock();
    std::cout <<"signaler exit" <<std::endl;
    __g_outLock.Unlock();

    return 0;
}

TestCase_Core_Thread_Semaphore::TestCase_Core_Thread_Semaphore()
{
}

TestCase_Core_Thread_Semaphore::~TestCase_Core_Thread_Semaphore()
{
}

int TestCase_Core_Thread_Semaphore::Run(int argc, char *argv[])
{
    std::cout <<"core/thread/semaphore test: " <<std::endl;

    // Create test value structure.
    __g_testVal = new __LLBC_SemTestType;
    __g_testVal->value = 0;

    // Create waiters.
    LLBC_NativeThreadHandle waiters[__g_waitersCount] = {LLBC_INVALID_NATIVE_THREAD_HANDLE};
    for(int i = 0; i < __g_waitersCount; i ++)
    {
        LLBC_CreateThread(&waiters[i], &WaiterThreadProc, reinterpret_cast<void *>(i));
    }

    // Create signaler.
    LLBC_NativeThreadHandle signaler = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    LLBC_CreateThread(&signaler, &SignalerThreadProc, NULL);

    // Join signaler.
    LLBC_JoinThread(signaler);

    // Join waiters.
    for(int i = 0; i < __g_waitersCount; i ++)
    {
        LLBC_JoinThread(waiters[i]);
    }

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    // Delete test value structure.
    delete __g_testVal;

    return 0;
}
