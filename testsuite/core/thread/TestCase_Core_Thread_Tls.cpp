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


#include "core/thread/TestCase_Core_Thread_Tls.h"

namespace
{
    static LLBC_Tls<long> __g_tls;
    static const long __g_threadNum = 5;
}

static void ThreadProc(void *arg)
{
    uint64 threadIndex = 0;
    memcpy(&threadIndex, &arg, sizeof(uint64));
    LLBC_PrintLn("thread %llu startup", threadIndex);

    __g_tls.SetValue(new long);
    (*__g_tls) = threadIndex;
    for(int i = 0; i < 5000000; ++i)
        (*__g_tls) += 1;

    LLBC_PrintLn("thread [%llu] tls value: %ld", threadIndex, *__g_tls);
}

TestCase_Core_Thread_Tls::TestCase_Core_Thread_Tls()
{
}

TestCase_Core_Thread_Tls::~TestCase_Core_Thread_Tls()
{
}

int TestCase_Core_Thread_Tls::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/thread/tls test");

    // Create threads.
    LLBC_NativeThreadHandle threads[__g_threadNum] = {LLBC_INVALID_NATIVE_THREAD_HANDLE};
    for(long i = 0; i < __g_threadNum; ++i)
    {
        void *threadArg = nullptr;
        memcpy(&threadArg, &i, sizeof(long));
        LLBC_CreateThread(&ThreadProc, threadArg, &threads[i]);
    }

    // Join threads.
    for(long i = 0; i < __g_threadNum; ++i)
    {
        LLBC_JoinThread(threads[i]);
    }

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return 0;
}
