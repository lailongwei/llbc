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


#include "core/thread/TestCase_Core_Thread_ThreadMgr.h"

static int ThreadProc(void *arg)
{
    LLBC_NativeThreadHandle nativeHandle = LLBC_ThreadManager::GetCurrentThread();
    LLBC_PrintLine("Hi, I'm thread %d", nativeHandle);

    LLBC_ThreadManager::Sleep(5000);

    return 0;
}

TestCase_Core_Thread_ThreadMgr::TestCase_Core_Thread_ThreadMgr()
{
}

TestCase_Core_Thread_ThreadMgr::~TestCase_Core_Thread_ThreadMgr()
{
}

int TestCase_Core_Thread_ThreadMgr::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/thread/ThreadManager test:");

    LLBC_PrintLine("Create threads:");
    LLBC_Handle groupHandle = LLBC_ThreadManagerSingleton->CreateThreads(2, &ThreadProc);
    if(groupHandle == LLBC_INVALID_HANDLE)
    {
        LLBC_PrintLine("Create threads failed, reason: %s", LLBC_FormatLastError());
    }
    else
    {
        LLBC_ThreadManager::Sleep(1000);
        LLBC_PrintLine("Create successed, group handle: %d, join it.", groupHandle);
        if(LLBC_ThreadManagerSingleton->WaitGroup(groupHandle) != LLBC_OK)
        {
            LLBC_PrintLine("Wait group failed, reason: %s", LLBC_FormatLastError());
        }
        else
        {
            LLBC_PrintLine("Wait group successed");
        }
    }

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
