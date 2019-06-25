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


#include "core/os/TestCase_Core_OS_Thread.h"

static int ThreadProc(void *arg)
{
    const char *threadDesc = reinterpret_cast<const char *>(arg);
    if(::strcmp(threadDesc, "thread 1") == 0)
    {
//      LLBC_Sleep(1000);
    }

    int loopVal = 10;
    for(int i = 0; i < loopVal; ++i)
    {
        LLBC_NativeThreadHandle handle = LLBC_GetCurrentThread();
#if LLBC_TARGET_PLATFORM_IPHONE || LLBC_TARGET_PLATFORM_MAC
        LLBC_PrintLine("Hello, I'm thread[%p]: %s", handle, threadDesc);
#else
        LLBC_PrintLine("Hello, I'm thread[%lu]: %s", handle, threadDesc);
#endif
        LLBC_PrintLine("\tThread priority: %d", LLBC_GetThreadPriority(handle));

        LLBC_Sleep(2000);
    }

    return 0;
}

TestCase_Core_OS_Thread::TestCase_Core_OS_Thread()
{
}

TestCase_Core_OS_Thread::~TestCase_Core_OS_Thread()
{
}

int TestCase_Core_OS_Thread::Run(int argc, char *argv[])
{
    std::cout <<"OS_Thread test:" <<std::endl;
    std::cout <<"Main thread: " <<LLBC_GetCurrentThread() <<std::endl;

    //! Non-Win32 specific.
#if LLBC_TARGET_PLATFORM_NON_WIN32
    std::cout <<"Sched policy SCHED_OTHER value: " <<SCHED_OTHER <<std::endl;
    std::cout <<"Sched policy SCHED_FIFO value: " <<SCHED_FIFO <<std::endl;
    std::cout <<"Sched policy SCHED_RR value: " <<SCHED_RR <<std::endl;
#endif

    // Create threads.
    LLBC_NativeThreadHandle handle1 = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    LLBC_NativeThreadHandle handle2 = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    std::cout <<"Create thread1" <<std::endl;
    if(LLBC_CreateThread(&handle1, ThreadProc, (void *)"thread 1") != LLBC_OK)
    {
        std::cerr <<"Create thread1 failed, err desc: " <<LLBC_FormatLastError() <<std::endl;
        return -1;
    }

    std::cout <<"thread1 priority: " <<LLBC_GetThreadPriority(handle1) <<std::endl;

    std::cout <<"Set thread1 priority to critical" <<std::endl;
    if(LLBC_SetThreadPriority(handle1, LLBC_ThreadPriority::Critical) != LLBC_OK)
    {
        std::cout <<"Set thread1 priority failed, err desc: " <<LLBC_FormatLastError() <<std::endl;
        return -1;
    }

    std::cout <<"thread1 new priority: " <<LLBC_GetThreadPriority(handle1) <<std::endl;

    std::cout <<"Create thread2" <<std::endl;
    if(LLBC_CreateThread(&handle2, ThreadProc, (void *)"thread 2") != LLBC_OK)
    {
        std::cerr <<"Create thread2 failed, err desc: " <<LLBC_FormatLastError() <<std::endl;
        return -1;
    }

    // Cancel thread1.
    LLBC_CancelThread(handle1);

    // Join it.
    std::cout <<"Join child threads terminated." <<std::endl;
    if(LLBC_JoinThread(handle1) != LLBC_OK)
    {
        std::cerr <<"Join thread1 failed, err desc: " <<LLBC_FormatLastError() <<std::endl;
        return -1;
    }
    if(LLBC_JoinThread(handle2) != LLBC_OK)
    {
        std::cerr <<"Join thread2 failed, err desc: " <<LLBC_FormatLastError() <<std::endl;
        return -1;
    }

    std::cout <<"Press any key to continue ... ..." <<std::endl;
    getchar();

    return 0;
}

