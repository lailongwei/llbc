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


#include "core/thread/TestCase_Core_Thread_Semaphore.h"

static const int __g_waitersCount = 5;

struct __LLBC_SemTestType
{
    int value;
    LLBC_Semaphore sem;
};
static __LLBC_SemTestType *__g_testVal;

static LLBC_SimpleLock __g_outLock;

static void WaiterThreadProc(void *arg)
{
    int threadIndex = 0;
    memcpy(&threadIndex, &arg, sizeof(int));

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
}

static void SignalerThreadProc(void *arg)
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
    for(long i = 0; i < __g_waitersCount; ++i)
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

    // Join waiters.
    for(long i = 0; i < __g_waitersCount; ++i)
    {
        LLBC_JoinThread(waiters[i]);
    }

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    // Delete test value structure.
    delete __g_testVal;

    return 0;
}
