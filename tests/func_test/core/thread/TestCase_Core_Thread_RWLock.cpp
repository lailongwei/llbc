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


#include "core/thread/TestCase_Core_Thread_RWLock.h"

static const long __g_readerCount = 5;
static const int __g_updateTimes = 50000;
static LLBC_SimpleLock __g_outLock;

static struct
{
    LLBC_RWLock lock;
    int value;
} __g_rwValue;

static void Reader_ThreadProc(void *arg)
{
    long threadIndex;
    memcpy(&threadIndex, &arg, sizeof(long));

    __g_outLock.Lock();
    std::cout <<"I'm reader: " <<threadIndex <<std::endl;
    __g_outLock.Unlock();

    for(int i = 1; ; ++i)
    {
        __g_rwValue.lock.ReadLock();
        if(i % 50000 == 0)
        {
            i = 0;
            __g_outLock.Lock();
            std::cout <<"reader [" <<threadIndex <<"] read data: " <<__g_rwValue.value <<std::endl;
            __g_outLock.Unlock();
        }

        __g_rwValue.lock.ReadUnlock();

        LLBC_Sleep(0);
    }
}

static void Writer_ThreadProc(void *arg)
{
    __g_outLock.Lock();
    std::cout <<"I'm writer" <<std::endl;
    __g_outLock.Unlock();

    for(int i = 0; i < __g_updateTimes; ++i)
    {
        __g_rwValue.lock.WriteLock();
        __g_rwValue.value += 1;
        __g_rwValue.lock.WriteUnlock();
        LLBC_Sleep(0);
    }

    __g_outLock.Lock();
    std::cout <<"Writer thread update value completed, final value: " <<__g_rwValue.value <<std::endl;
    __g_outLock.Unlock();
}

TestCase_Core_Thread_RWLock::TestCase_Core_Thread_RWLock()
{
}

TestCase_Core_Thread_RWLock::~TestCase_Core_Thread_RWLock()
{
}

int TestCase_Core_Thread_RWLock::Run(int argc, char *argv[])
{
    std::cout <<"core/thread/rwlock test: " <<std::endl;

    // Create readers.
    LLBC_NativeThreadHandle readers[__g_readerCount] = {LLBC_INVALID_NATIVE_THREAD_HANDLE};
    for(long i = 0; i < __g_readerCount; ++i)
    {
        void *threadArg = nullptr;
        memcpy(&threadArg, &i, sizeof(long));
        LLBC_CreateThread(&Reader_ThreadProc, threadArg, &readers[i]);
    }

    // Create writer.
    LLBC_NativeThreadHandle writer = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    LLBC_CreateThread(&Writer_ThreadProc, nullptr, &writer);

    // Join writer.
    LLBC_JoinThread(writer);

    // Cancel readers and join it.
    for(long i = 0; i < __g_readerCount; ++i)
    {
        LLBC_CancelThread(readers[i]);
        LLBC_JoinThread(readers[i]);
    }

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}
