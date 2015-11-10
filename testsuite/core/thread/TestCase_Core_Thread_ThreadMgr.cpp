/**
 * @file    TestCase_Core_Thread_ThreadMgr.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/20
 * @version 1.0
 *
 * @brief
 */

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
        if(LLBC_ThreadManagerSingleton->WaitGroup(groupHandle) != LLBC_RTN_OK)
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
