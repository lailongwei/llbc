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


#include <signal.h>
#include "core/thread/TestCase_Core_Thread_ThreadMgr.h"

TestCase_Core_Thread_ThreadMgr::TestCase_Core_Thread_ThreadMgr()
{
}

TestCase_Core_Thread_ThreadMgr::~TestCase_Core_Thread_ThreadMgr()
{
}

int TestCase_Core_Thread_ThreadMgr::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/thread/ThreadManager test:");

    LLBC_ReturnIf(Test_StaticMethods() != LLBC_OK, LLBC_FAILED);

    LLBC_ReturnIf(Test_KillEntryThread() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(Test_WaitAndCancelInEnryThread() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(Test_SuspendAndResumeEntryThread() != LLBC_OK, LLBC_FAILED);
    
    LLBC_ReturnIf(Test_CreateThreads() != LLBC_OK, LLBC_FAILED);

    LLBC_ReturnIf(Test_KillThreads() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(Test_WaitThreads() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(Test_CancelThreads() != LLBC_OK, LLBC_FAILED);
    LLBC_ReturnIf(Test_SuspendAndResumeThreads() != LLBC_OK, LLBC_FAILED);

    LLBC_PrintLn("Press any key to continue ...");
    (void)getchar();

    return 0;
}

int TestCase_Core_Thread_ThreadMgr::Test_StaticMethods()
{
    LLBC_SpinLock syncLock;
    LLBC_PrintLn("Static Methods test:");
    auto execCall = [&syncLock](void *arg) {
        LLBC_LockGuard guard(syncLock);

        LLBC_PrintLn("- Call in %s thread:",
                     LLBC_ThreadMgr::InLLBCEntryThread() ? "entry" : "non-entry");
        LLBC_PrintLn("  - Cur thread id:%d", LLBC_ThreadMgr::CurThreadId());
        LLBC_PrintLn("  - Cur thread handle:%d", LLBC_ThreadMgr::CurThreadHandle());
        LLBC_PrintLn("  - Cur native thread handle:%llu", (uint64)LLBC_ThreadMgr::CurNativeThreadHandle());
        LLBC_PrintLn("  - Cur thread group handle:%d", LLBC_ThreadMgr::CurGroupHandle());
    };

    // Call in entry thread.
    execCall(nullptr);

    // Call in non-entry threads.
    auto threadMgr = LLBC_ThreadMgrSingleton;
    LLBC_Handle group = threadMgr->CreateThreads(5, execCall, &syncLock);
    LLBC_LogAndReturnIf(group == LLBC_INVALID_HANDLE, Error, LLBC_FAILED);
    LLBC_LogAndReturnIf(threadMgr->WaitGroup(group) != LLBC_OK, Error, LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Core_Thread_ThreadMgr::Test_WaitAndCancelInEnryThread()
{
    // Check in entry thread.
    LLBC_PrintLn("Wait&Cancel entry thread test:");
    LLBC_LogAndReturnIf(!LLBC_ThreadMgr::InLLBCEntryThread(), Error, LLBC_FAILED);

    // Check entry thread handle.
    const LLBC_Handle curThreadHandle = LLBC_ThreadMgr::CurThreadHandle();
    LLBC_PrintLn("- Entry thread handle:%d", curThreadHandle);
    LLBC_LogAndReturnIf(curThreadHandle != LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE,
                        Error,
                        LLBC_FAILED);
    // Check entry thread group handle.
    const LLBC_Handle curGroupHandle = LLBC_ThreadMgr::CurGroupHandle();
    LLBC_PrintLn("- Entry thread group handle:%d", curGroupHandle);
    LLBC_LogAndReturnIf(curGroupHandle != LLBC_CFG_THREAD_ENTRY_THREAD_GROUP_HANDLE,
                        Error,
                        LLBC_FAILED);

    // Try wait entry thread.
    int waitRet = LLBC_ThreadMgrSingleton->Wait(curThreadHandle);
    LLBC_PrintLn("- Wait entry thread handle ret:%d, err:%s",
                 waitRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(waitRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);
    // Try wait entry group.
    waitRet = LLBC_ThreadMgrSingleton->WaitGroup(curGroupHandle);
    LLBC_PrintLn("- Wait entry group handle ret:%d, err:%s",
                 waitRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(waitRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);

    // Try cancel entry thread.
    int cancelRet = LLBC_ThreadMgrSingleton->Cancel(curThreadHandle);
    LLBC_PrintLn("- Cancel entry thread handle ret:%d, err:%s",
                 cancelRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(cancelRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);
    // Try cancel entry group.
    cancelRet = LLBC_ThreadMgrSingleton->CancelGroup(curGroupHandle);
    LLBC_PrintLn("- Cancel entry group handle ret:%d, err:%s",
                 cancelRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(cancelRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Core_Thread_ThreadMgr::Test_SuspendAndResumeEntryThread()
{
    LLBC_PrintLn("Suspend&Resume entry thread test:");

    // Make sure in entry thread.
    auto threadMgr = LLBC_ThreadMgrSingleton;
    LLBC_LogAndReturnIf(!LLBC_ThreadMgr::InLLBCEntryThread(), Error, LLBC_FAILED);

    // Try suspend entry thread.
    const LLBC_Handle thread = LLBC_ThreadMgr::CurThreadHandle();
    int suspendRet = threadMgr->Suspend(thread);
    LLBC_PrintLn("- Suspend entry thread ret:%d, err:%s",
                 suspendRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(suspendRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);
    // Try suspend entry thread group.
    const LLBC_Handle group = LLBC_ThreadMgr::CurGroupHandle();
    suspendRet = threadMgr->SuspendGroup(group);
    LLBC_PrintLn("- Suspend entry thread group ret:%d, err:%s",
                 suspendRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(suspendRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);

    // Try resume entry thread.
    int resumeRet = threadMgr->Resume(thread);
    LLBC_PrintLn("- Resume entry thread ret:%d, err:%s",
                 resumeRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(suspendRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);
    // Try resume entry thread group.
    suspendRet = threadMgr->ResumeGroup(group);
    LLBC_PrintLn("- Resume entry thread group ret:%d, err:%s",
                 resumeRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(suspendRet == LLBC_OK || LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Core_Thread_ThreadMgr::Test_KillEntryThread()
{
    LLBC_PrintLn("Kill entry thread test:");
    LLBC_LogAndReturnIf(!LLBC_ThreadMgr::InLLBCEntryThread(), Error, LLBC_FAILED);

    // Try kill entry thread.
    LLBC_ThreadMgr *threadMgr = LLBC_ThreadMgrSingleton;
    const LLBC_Handle curThreadHandle = LLBC_ThreadMgr::CurThreadHandle();
    auto killRet = threadMgr->Kill(curThreadHandle, 9);
    LLBC_PrintLn("- Kill entry thread ret:%d, err:%s",
                 killRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(killRet == LLBC_OK ||
                            LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);

    // Try kill entry thread group.
    const LLBC_Handle curGroupHandle = LLBC_ThreadMgr::CurGroupHandle();
    killRet = threadMgr->KillGroup(curGroupHandle, 9);
    LLBC_PrintLn("- Kill entry thread group ret:%d, err:%s",
                 killRet, LLBC_FormatLastError());
    LLBC_LogAndReturnIf(killRet == LLBC_OK ||
                            LLBC_GetLastError() != LLBC_ERROR_NOT_ALLOW,
                        Error,
                        LLBC_FAILED);

    return LLBC_OK;
}

int TestCase_Core_Thread_ThreadMgr::Test_CreateThreads()
{
    static constexpr int threadNum = 5;

    LLBC_PrintLn("Test create threads:");

    // Define thread entry.
    sint64 value = 0;
    LLBC_SpinLock syncLock;
    auto threadEntry = [&syncLock, &value](void *arg)
    {
        LLBC_LockGuard guard(syncLock);

        LLBC_PrintLn("%s: ThreadInfo:", __FUNCTION__);
        LLBC_PrintLn("- is llbc thread?:%d", LLBC_ThreadMgr::InLLBCThread());
        LLBC_PrintLn("- is llbc entry thread?:%d", LLBC_ThreadMgr::InLLBCEntryThread());
        LLBC_PrintLn("- ThreadId:%d", static_cast<int>(LLBC_ThreadMgr::CurThreadId()));
        LLBC_PrintLn("- Thread Handle:%d", LLBC_ThreadMgr::CurThreadHandle());
        LLBC_PrintLn("- Native Thread Handle:%llu", (uint64)LLBC_ThreadMgr::CurNativeThreadHandle());
        LLBC_PrintLn("- Thread Group Handle:%d", LLBC_ThreadMgr::CurGroupHandle());

        for (int i = 0; i < 10000000; ++i)
            LLBC_AtomicFetchAndAdd(&value, 1);
    };

    // Create threads
    syncLock.Lock();
    auto threadMgr = LLBC_ThreadMgrSingleton;
    LLBC_PrintLn("Create %d threads for test ...", threadNum);
    LLBC_Handle group = threadMgr->CreateThreads(threadNum, threadEntry, this);
    if (group == LLBC_INVALID_HANDLE)
    {
        syncLock.Unlock();
        LLBC_FilePrintLn(stderr, "Create threads failed, err:%s", LLBC_FormatLastError());

        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    // Get thread handles.
    std::vector<LLBC_Handle> handles;
    LLBC_PrintLn("Get group thread handles ...");
    if (threadMgr->GetGroupThreadHandles(group, handles) != LLBC_OK)
    {
        syncLock.Unlock();
        LLBC_FilePrintLn(stderr, "Get group thread handles failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    LLBC_LogAndReturnIf(handles.size() != threadNum, Error, LLBC_FAILED);
    for (size_t i = 0; i < handles.size(); ++i)
        LLBC_PrintLn("- thread 0: %d", handles[i]);

    // Wait thread group.
    syncLock.Unlock();
    LLBC_PrintLn("Wait thread group finish ...");
    if (threadMgr->WaitGroup(group) != LLBC_OK &&
        LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
    {
        LLBC_FilePrintLn(stderr, "Wait failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done, value:%lld", value);

    return LLBC_OK;
}

int TestCase_Core_Thread_ThreadMgr::Test_KillThreads()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_PrintLn("Kill threads test will raise process level "
                 "crash on non-win32 platform, skip test...");
    return LLBC_OK;
#else // Win32
    static constexpr int threadNum = 10;

    LLBC_PrintLn("Kill threads test:");

    // Define kill-threads thread entry.
    volatile int runningThreadNum = 0;
    auto threadEntry = [&runningThreadNum](void *) {
        LLBC_PrintLn("I am thread %d, waiting for kill ...",
                     LLBC_ThreadMgr::CurThreadHandle());

        LLBC_AtomicFetchAndAdd(&runningThreadNum, 1);
        while (true)
            LLBC_Sleep(LLBC_Rand(500, 2000));
    };

    // Create test threads.
    auto threadMgr = LLBC_ThreadMgrSingleton;
    LLBC_PrintLn("Create %d threads for test ...", threadNum);
    auto group = threadMgr->CreateThreads(threadNum, threadEntry, nullptr);
    LLBC_LogAndReturnIf(group == LLBC_INVALID_HANDLE, Error, LLBC_FAILED);
    while (runningThreadNum != threadNum)
        LLBC_Sleep(1);
    LLBC_PrintLn("Done, group handle:%d", group);

    // Choose thread for kill.
    LLBC_Sleep(500);
    std::vector<LLBC_Handle> handles;
    threadMgr->GetGroupThreadHandles(group, handles);
    LLBC_Handle chooseThread = handles[LLBC_Rand(0, threadNum)];
    LLBC_PrintLn("Choose thread %d for kill ...", chooseThread);
    if (threadMgr->Kill(chooseThread, SIGABRT) != LLBC_OK)
    {
        LLBC_PrintLn("Kill thread failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    // After kill one thread, check group thread numbers.
    handles.clear();
    threadMgr->GetGroupThreadHandles(group, handles);
    if (handles.size() != threadNum - 1)
    {
        LLBC_FilePrintLn(stderr,
                         "After kill thread, threadNum[%lu] != %d",
                         handles.size(), threadNum - 1);
        return LLBC_FAILED;
    }

    // Kill thread group.
    LLBC_PrintLn("Kill thread group ...");
    if (threadMgr->KillGroup(group, SIGABRT) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Kill thread group failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    // After kill thread group, check group threads.
    handles.clear();
    int ret = threadMgr->GetGroupThreadHandles(group, handles);
    if (ret == LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "After kill thread group, "
                         "Call ThreadMgr::GetGroupThreadHandles() return OK!");
        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);

        return LLBC_FAILED;
    }
    else if (LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
    {
        LLBC_FilePrintLn(stderr,
                         "After kill thread group, Call "
                         "ThreadMgr::GetGroupThreadHandles() return err[%d] != LLBC_ERROR_NOT_FOUND",
                         LLBC_GetLastError());
        return LLBC_FAILED;
    }

    // Kill all.
    runningThreadNum = 0;
    LLBC_PrintLn("Create %d threads for test kill all ...", threadNum);
    (void)threadMgr->CreateThreads(threadNum, threadEntry);
    LLBC_PrintLn("Done!");

    while (runningThreadNum != threadNum)
        LLBC_Sleep(1);
    LLBC_PrintLn("Kill all threads ...");
    if (threadMgr->KillAll(SIGABRT) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Kill all threads failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    return LLBC_OK;
#endif // Non-Win32
}

int TestCase_Core_Thread_ThreadMgr::Test_WaitThreads()
{
    static constexpr int threadNum = 10;

    LLBC_PrintLn("Wait threads test:");

    // Define thread entry.
    auto threadEntry = [](void *)
    {
        const int sleepMs = LLBC_Rand(500, 2000);
        LLBC_PrintLn("%-3d: Sleep %d ms", LLBC_ThreadMgr::CurThreadHandle(), sleepMs);

        LLBC_Sleep(sleepMs);
        LLBC_PrintLn("%-3d: Finish", LLBC_ThreadMgr::CurThreadHandle());
    };

    // Create threads.
    auto threadMgr = LLBC_ThreadMgrSingleton;
    LLBC_PrintLn("Create %d threads ...", threadNum);
    auto group = threadMgr->CreateThreads(threadNum, threadEntry);
    LLBC_LogAndReturnIf(group == LLBC_INVALID_HANDLE, Error, LLBC_FAILED);
    LLBC_PrintLn("Done, group handle:%d", group);

    // Choose thread for wait.
    std::vector<LLBC_Handle> threadHandles;
    threadMgr->GetGroupThreadHandles(group, threadHandles);
    auto chooseThread = threadHandles[LLBC_Rand(static_cast<int>(threadHandles.size()))];
    LLBC_PrintLn("Choose thread %d for wait ...", chooseThread);
    if (threadMgr->Wait(chooseThread) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Wait thread failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    // Wait group.
    LLBC_PrintLn("Wait group ...");
    if (threadMgr->WaitGroup(group) != LLBC_OK && LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
    {
        LLBC_FilePrintLn(stderr, "Wait group failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    // Wait all.
    LLBC_PrintLn("Create %d threads for test wait all ...", threadNum);
    threadMgr->CreateThreads(threadNum, threadEntry);
    LLBC_PrintLn("Done!");

    LLBC_PrintLn("Wait all threads ...");
    if (threadMgr->WaitAll() != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Wait all threads failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    return LLBC_OK;
}

int TestCase_Core_Thread_ThreadMgr::Test_CancelThreads()
{
    static constexpr int threadNum = 10;

    LLBC_PrintLn("Test cancel threads:");

    // Define thread entry.
    volatile int runningThreadNum = 0;
    auto threadEntry = [&runningThreadNum](void *) {
        LLBC_PrintLn("Thread %d is running, waiting for cancel ...",
                     LLBC_ThreadMgr::CurThreadHandle());

        LLBC_AtomicFetchAndAdd(&runningThreadNum, 1);

        while (true)
            LLBC_Sleep(10);
    };

    // Create test threads.
    auto threadMgr = LLBC_ThreadMgrSingleton;
    LLBC_PrintLn("Create %d threads for test ...", threadNum);
    LLBC_Handle group = threadMgr->CreateThreads(threadNum, threadEntry);
    LLBC_LogAndReturnIf(group == LLBC_INVALID_HANDLE, Error, LLBC_FAILED);
    while (runningThreadNum != threadNum)
        LLBC_Sleep(1);
    LLBC_PrintLn("Done, group handle:%d", group);

    // Choose thread for cancel.
    std::vector<LLBC_Handle> threads;
    threadMgr->GetGroupThreadHandles(group, threads);
    LLBC_Handle chooseThread =
        threads[LLBC_Rand(static_cast<int>(threads.size()))];
    LLBC_PrintLn("Choose thread %d for cancel ...", chooseThread);
    if (threadMgr->Cancel(chooseThread) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Cancel thread failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    // Cancel group.
    LLBC_PrintLn("Cancel group ...");
    if (threadMgr->CancelGroup(group) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Cancel group failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    // Cancel all.
    runningThreadNum = 0;
    LLBC_PrintLn("Create %d threads for test cancel all ...", threadNum);
    threadMgr->CreateThreads(threadNum, threadEntry);
    while (runningThreadNum != threadNum)
        LLBC_Sleep(1);
    LLBC_PrintLn("Done!");

    LLBC_PrintLn("Cancel all threads ...");
    if (threadMgr->CancelAll() != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr, "Cancel all threads failed, err:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done!");

    return LLBC_OK;
}

int TestCase_Core_Thread_ThreadMgr::Test_SuspendAndResumeThreads()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_PrintLn("For, Suspend or resume threads operation "
                 "not supported in non-win32 platform");
    return LLBC_OK;
#endif

    static constexpr int threadNum = 2;

    LLBC_PrintLn("Test Suspend/Resume threads:");

    // Define thread entry.
    volatile bool stopFlag = false;
    volatile int runningThreadsNum = 0;
    auto threadEntry = [&stopFlag, &runningThreadsNum](void *) {
        LLBC_PrintLn("Thread thread %d is running ...",
                     LLBC_ThreadMgr::CurThreadHandle());

        LLBC_AtomicFetchAndAdd(&runningThreadsNum, 1);

        while (!stopFlag)
        {
            LLBC_Sleep(500);
            LLBC_PrintLn("%d waiting for suspend ...",
                         LLBC_ThreadMgr::CurThreadHandle());
        }
    };

    // Create test threads.
    auto threadMgr = LLBC_ThreadMgrSingleton;
    LLBC_PrintLn("Create %d threads for test Suspend/Rsume thread ...", threadNum);
    LLBC_Handle group = threadMgr->CreateThreads(threadNum, threadEntry);
    while (runningThreadsNum != threadNum)
        LLBC_Sleep(1);
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    // Choose thread for test.
    std::vector<LLBC_Handle> threads;
    threadMgr->GetGroupThreadHandles(group, threads);
    LLBC_Handle chooseThread = threads[LLBC_Rand(static_cast<int>(threads.size()))];
    LLBC_PrintLn("Choose thread %d for Suspend/Resume ...", chooseThread);
    LLBC_PrintLn("- Suspend ...");
    if (threadMgr->Suspend(chooseThread) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Suspend thread failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    LLBC_PrintLn("- Resume...");
    if (threadMgr->Resume(chooseThread) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Resume thread failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    // Suspend/Resume group.
    LLBC_PrintLn("Test Suspend/Resume group ...");
    LLBC_PrintLn("- Suspend ...");
    if (threadMgr->SuspendGroup(group) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Suspend group failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    LLBC_PrintLn("- Resume ...");
    if (threadMgr->ResumeGroup(group) != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Resume group failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    LLBC_PrintLn("Stop group threads...");
    stopFlag = true;
    LLBC_LogAndReturnIf(
        threadMgr->WaitGroup(group) != LLBC_OK, Error, LLBC_FAILED);
    LLBC_PrintLn("Done!");

    // Suspend/Resume all test.
    stopFlag = false;
    runningThreadsNum = 0;
    LLBC_PrintLn("Create %d threads for test Suspend/Resume all ...", threadNum);
    group = threadMgr->CreateThreads(threadNum, threadEntry);
    while (runningThreadsNum != threadNum)
        LLBC_Sleep(1);
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    LLBC_PrintLn("- Suspend ...");
    if (threadMgr->SuspendAll() != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Suspend all failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    LLBC_PrintLn("- Resume ...");
    if (threadMgr->ResumeAll() != LLBC_OK)
    {
        LLBC_FilePrintLn(stderr,
                         "Resume all failed, err:%s",
                         LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    LLBC_PrintLn("Done, sleep 5 seconds to watch ...");
    LLBC_Sleep(5000);

    LLBC_PrintLn("Stop all test threads ...");
    stopFlag = true;
    LLBC_LogAndReturnIf(threadMgr->WaitAll() != LLBC_OK, Error, LLBC_FAILED);

    return LLBC_OK;
}
