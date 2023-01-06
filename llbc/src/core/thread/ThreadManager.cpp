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


#include "llbc/common/Export.h"

#include "llbc/core/os/OS_Thread.h"

#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/timer/TimerScheduler.h"
#include "llbc/core/objectpool/ObjectPool.h"
#include "llbc/core/objbase/ObjBase.h"

#include "llbc/core/thread/Guard.h"
#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/thread/Task.h"
#include "llbc/core/thread/ThreadDescriptor.h"
#include "llbc/core/thread/ThreadGroupDescriptor.h"
#include "llbc/core/thread/ThreadManager.h"

__LLBC_INTERNAL_NS_BEGIN

extern "C" {
struct __LLBC_ThreadMgr_Thread_Arg
{
    LLBC_NS LLBC_ThreadArg realArg;
    LLBC_NS LLBC_ThreadProc realProc;
    LLBC_NS LLBC_Handle threadHandle;
    LLBC_NS LLBC_ThreadManager *threadMgr;
};
}

static LLBC_NS LLBC_ThreadRtn __LLBC_ThreadMgr_ThreadEntry(LLBC_NS LLBC_ThreadArg arg)
{
    // Get thread arguments.
    __LLBC_ThreadMgr_Thread_Arg *threadArg = 
        reinterpret_cast<__LLBC_ThreadMgr_Thread_Arg *>(arg);
    ASSERT(threadArg && "llbc library internal error, threadArg nullptr!");

    LLBC_NS LLBC_ThreadArg realArg = threadArg->realArg;
    LLBC_NS LLBC_ThreadProc realProc = threadArg->realProc;
    LLBC_NS LLBC_Handle threadHandle = threadArg->threadHandle;
    LLBC_NS LLBC_ThreadManager *threadMgr = threadArg->threadMgr;

    // Set TLS.
    LLBC_NS __LLBC_LibTls *tls = LLBC_NS __LLBC_GetLibTls();
    tls->coreTls.llbcThread = true;
    tls->coreTls.threadHandle = threadArg->threadHandle;
    tls->coreTls.threadId = LLBC_NS LLBC_GetCurrentThreadId();
#if LLBC_TARGET_PLATFORM_NON_WIN32
    tls->coreTls.nativeThreadHandle = LLBC_NS LLBC_GetCurrentThread();
#else // LLBC_TARGET_PLATFORM_WIN32
    HANDLE pseudoHandle = ::GetCurrentThread();
    ::DuplicateHandle(::GetCurrentProcess(),
                      pseudoHandle,
                      ::GetCurrentProcess(),
                      &tls->coreTls.nativeThreadHandle,
                      0,
                      FALSE,
                      DUPLICATE_SAME_ACCESS);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

    // Setup core tls some components.
    tls->coreTls.safetyObjectPool = new LLBC_NS LLBC_SafetyObjectPool;
    tls->coreTls.unsafetyObjectPool = new LLBC_NS LLBC_UnsafetyObjectPool;
    tls->coreTls.timerScheduler = new LLBC_NS LLBC_TimerScheduler;

    // Setup objbase tls some components.
    tls->objbaseTls.poolStack = new LLBC_NS LLBC_AutoReleasePoolStack;
    new LLBC_NS LLBC_AutoReleasePool;

    // Delete arg.
    delete threadArg;

    // Notify thread manager thread startup.
    threadMgr->OnThreadStartup(threadHandle);

    // Call thread proc.
    LLBC_NS LLBC_ThreadRtn rtn = (*realProc)(realArg);

    // Notify thread manager thread terminated.
    threadMgr->OnThreadTerminate(threadHandle);

    // Cleanup objbase tls components.
    delete reinterpret_cast<LLBC_NS LLBC_AutoReleasePoolStack *>(tls->objbaseTls.poolStack); tls->objbaseTls.poolStack = nullptr;

    // Cleanup core tls components.
    delete reinterpret_cast<LLBC_NS LLBC_TimerScheduler *>(tls->coreTls.timerScheduler); tls->coreTls.timerScheduler = nullptr;
    delete reinterpret_cast<LLBC_NS LLBC_SafetyObjectPool *>(tls->coreTls.safetyObjectPool); tls->coreTls.safetyObjectPool = nullptr;
    delete reinterpret_cast<LLBC_NS LLBC_UnsafetyObjectPool *>(tls->coreTls.unsafetyObjectPool); tls->coreTls.unsafetyObjectPool = nullptr;

#if LLBC_TARGET_PLATFORM_WIN32
    ::CloseHandle(tls->coreTls.nativeThreadHandle);
#endif // LLBC_TARGET_PLATFORM_WIN32
    LLBC_NS __LLBC_ResetLibTls();

    return rtn;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_ThreadManager::LLBC_ThreadManager()
: _maxThreadHandle(0)
, _maxGroupHandle(0)
{
    memset(_groups, 0, sizeof(_groups));
    memset(_threads, 0, sizeof(_threads));
}

LLBC_ThreadManager::~LLBC_ThreadManager()
{
    WaitAll();
}

LLBC_Handle LLBC_ThreadManager::CreateThreads(int threadNum,
                                              LLBC_ThreadProc proc,
                                              LLBC_ThreadArg arg,
                                              int flags,
                                              int priority,
                                              const int stackSize[],
                                              LLBC_Task *task,
                                              LLBC_Handle groupHandle,
                                              LLBC_NativeThreadHandle nativeHandles[],
                                              LLBC_Handle handles[])
{
    if (threadNum <= 0 || !proc)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_HANDLE;
    }

    LLBC_LockGuard guard(_lock);
    if (groupHandle == LLBC_INVALID_HANDLE)
        groupHandle =++ _maxGroupHandle;
    
    for (int i = 0; i < threadNum; ++i)
    {
        if (CreateThread_NonLock(proc,
                                 arg,
                                 flags,
                                 priority,
                                 stackSize ? stackSize[i] : LLBC_CFG_THREAD_DFT_STACK_SIZE,
                                 task,
                                 groupHandle,
                                 nativeHandles ? &nativeHandles[i] : nullptr,
                                 handles ? &handles[i] : nullptr) == LLBC_INVALID_HANDLE)
            return LLBC_FAILED;
    }

    return groupHandle;
}

LLBC_Handle LLBC_ThreadManager::CreateThread(LLBC_ThreadProc proc,
                                             LLBC_ThreadArg arg,
                                             int flags,
                                             int priority,
                                             int stackSize,
                                             LLBC_Task *task,
                                             LLBC_Handle groupHandle,
                                             LLBC_NativeThreadHandle *nativeHandle,
                                             LLBC_Handle *handle)
{
    if (!proc)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_HANDLE;
    }

    LLBC_LockGuard guard(_lock);

    return CreateThread_NonLock(proc,
                                arg,
                                flags,
                                priority,
                                stackSize,
                                task,
                                groupHandle,
                                nativeHandle,
                                handle);
}

void LLBC_ThreadManager::Sleep(int milliSecs)
{
    LLBC_Sleep(milliSecs);
}

LLBC_NativeThreadHandle LLBC_ThreadManager::GetCurrentThread()
{
    return LLBC_GetCurrentThread();
}

LLBC_NativeThreadHandle LLBC_ThreadManager::GetNativeThreadHandle(LLBC_Handle handle) const
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_NATIVE_THREAD_HANDLE;
    }

    LLBC_ThreadManager *nonConstThis = const_cast<LLBC_ThreadManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_INVALID_NATIVE_THREAD_HANDLE;
    }

    return threadDesc->GetNativeHandle();
}

LLBC_Handle LLBC_ThreadManager::GetGroupHandle(LLBC_Handle handle) const
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_HANDLE;
    }

    LLBC_ThreadManager *nonConstThis = const_cast<LLBC_ThreadManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_INVALID_HANDLE;
    }

    return threadDesc->GetGroupHandle();
}

LLBC_Task *LLBC_ThreadManager::GetTask(LLBC_Handle handle) const
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return nullptr;
    }

    LLBC_ThreadManager *nonConstThis = const_cast<LLBC_ThreadManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    return threadDesc->GetTask();
}

int LLBC_ThreadManager::GetStackSize(LLBC_Handle handle) const
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return -1;
    }

    LLBC_ThreadManager *nonConstThis = const_cast<LLBC_ThreadManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return -1;
    }

    return threadDesc->GetStackSize();
}

int LLBC_ThreadManager::GetPriority(LLBC_Handle handle) const
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return -1;
    }

    LLBC_ThreadManager *nonConstThis = const_cast<LLBC_ThreadManager *>(this);
    LLBC_LockGuard guard(nonConstThis->_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return -1;
    }

    return threadDesc->GetPriority();
}

int LLBC_ThreadManager::SetPriority(LLBC_Handle handle, int priority)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (threadDesc->GetPriority() == priority)
    {
        return LLBC_OK;
    }

    int rtn = LLBC_SetThreadPriority(threadDesc->GetNativeHandle(), priority);
    if (rtn == LLBC_OK)
        threadDesc->SetPriority(priority);

    return rtn;
}

int LLBC_ThreadManager::Wait(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _lock.Lock();
    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        _lock.Unlock();

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (threadDesc->GetState() != LLBC_ThreadState::Running &&
        threadDesc->GetState() != LLBC_ThreadState::Suspended)
    {
        _lock.Unlock();

        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    LLBC_NativeThreadHandle nativeHandle = threadDesc->GetNativeHandle();

    _lock.Unlock();

    int rtn = LLBC_JoinThread(nativeHandle);
    if (rtn == LLBC_OK)
    {
        LLBC_LockGuard guard(_lock);
        if ((threadDesc = FindThreadDescriptor(handle)))
        {

#if LLBC_TARGET_PLATFORM_WIN32
            if (threadDesc->GetFlags() | LLBC_ThreadFlag::Joinable)
                ::CloseHandle(threadDesc->GetNativeHandle());
#endif
            RemoveThreadDescriptor(handle);
        }
    }

    return rtn;
}

int LLBC_ThreadManager::WaitTask(LLBC_Task *task)
{
    if (UNLIKELY(!task))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::vector<LLBC_Handle> taskThreads;
    task->GetTaskThreads(taskThreads);

    for (size_t i = 0; i != taskThreads.size(); ++i)
    {
        if (Wait(taskThreads[i]) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_NOT_FOUND)
                continue;

            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::WaitGroup(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _lock.Lock();

    LLBC_ThreadGroupDescriptor *groupDesc = 
        FindThreadGroupDescriptor(handle);
    if (!groupDesc)
    {
        _lock.Unlock();

        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    std::vector<LLBC_Handle> willWaitThreads;
    LLBC_ThreadDescriptor *threadDesc = 
        groupDesc->FindFirstThreadDescriptor();
    while (threadDesc)
    {
        willWaitThreads.push_back(threadDesc->GetHandle());
        threadDesc = threadDesc->GetGroupThreadNext();
    }

    _lock.Unlock();

    for (size_t i = 0; i < willWaitThreads.size(); ++i)
    {
        if (Wait(willWaitThreads[i]) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::WaitAll()
{
    _lock.Lock();

    std::vector<LLBC_Handle> willWaitThreads;
    for (int i = 0; i < LLBC_CFG_THREAD_MAX_THREAD_NUM; ++i)
    {
        LLBC_ThreadDescriptor *threadDesc = _threads[i];
        while (threadDesc)
        {
            willWaitThreads.push_back(threadDesc->GetHandle());
            threadDesc = threadDesc->GetThreadNext();
        }
    }

    _lock.Unlock();

    for (size_t i = 0; i < willWaitThreads.size(); ++i)
    {
        if (Wait(willWaitThreads[i]) != LLBC_OK)
            return LLBC_FAILED;
    }
    return LLBC_OK;
}

int LLBC_ThreadManager::Suspend(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (threadDesc->GetState() == LLBC_ThreadState::Suspended)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }
    else if (threadDesc->GetState() != LLBC_ThreadState::Running)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    int rtn = LLBC_SuspendThread(threadDesc->GetNativeHandle());
    if (rtn == LLBC_OK)
        threadDesc->SetState(LLBC_ThreadState::Suspended);

    return rtn;
}

int LLBC_ThreadManager::SuspendTask(LLBC_Task *task)
{
    if (UNLIKELY(!task))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::vector<LLBC_Handle> taskThreads;
    task->GetTaskThreads(taskThreads);

    for (size_t i = 0; i < taskThreads.size(); ++i)
    {
        if (Suspend(taskThreads[i]) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_NOT_FOUND)
                continue;

            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::SuspendGroup(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadGroupDescriptor *groupDesc = 
        FindThreadGroupDescriptor(handle);
    if (!groupDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    LLBC_ThreadDescriptor *threadDesc = 
        groupDesc->FindFirstThreadDescriptor();
    while (threadDesc)
    {
        LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
        threadDesc = threadDesc->GetGroupThreadNext();

        if (Suspend(tmpThreadDesc->GetHandle()) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::SuspendAll()
{
    for (int i = 0; i < LLBC_CFG_THREAD_MAX_THREAD_NUM; ++i)
    {
        LLBC_ThreadDescriptor *threadDesc = _threads[i];
        while (threadDesc)
        {
            LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
            threadDesc = threadDesc->GetThreadNext();

            if (Suspend(tmpThreadDesc->GetHandle()) != LLBC_OK)
                return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::Resume(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (threadDesc->GetState() == LLBC_ThreadState::Running)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }
    else if (threadDesc->GetState() != LLBC_ThreadState::Suspended)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    int rtn = LLBC_ResumeThread(threadDesc->GetNativeHandle());
    if (rtn == LLBC_OK)
        threadDesc->SetState(LLBC_ThreadState::Running);

    return rtn;
}

int LLBC_ThreadManager::ResumeTask(LLBC_Task *task)
{
    if (UNLIKELY(!task))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::vector<LLBC_Handle> taskThreads;
    task->GetTaskThreads(taskThreads);

    for (size_t i = 0; i < taskThreads.size(); ++i)
    {
        if (Resume(taskThreads[i]) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_NOT_FOUND)
                continue;

            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::ResumeGroup(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadGroupDescriptor *groupDesc = 
        FindThreadGroupDescriptor(handle);
    if (!groupDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    LLBC_ThreadDescriptor *threadDesc = 
        groupDesc->FindFirstThreadDescriptor();
    while (threadDesc)
    {
        LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
        threadDesc = threadDesc->GetGroupThreadNext();

        if (Resume(tmpThreadDesc->GetHandle()) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::ResumeAll()
{
    for (int i = 0; i < LLBC_CFG_THREAD_MAX_THREAD_NUM; ++i)
    {
        LLBC_ThreadDescriptor *threadDesc = _threads[i];
        while (threadDesc)
        {
            LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
            threadDesc = threadDesc->GetThreadNext();

            if (Resume(tmpThreadDesc->GetHandle()) != LLBC_OK)
                return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::Cancel(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (threadDesc->GetState() != LLBC_ThreadState::Running &&
        threadDesc->GetState() != LLBC_ThreadState::Suspended)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    int rtn = LLBC_CancelThread(threadDesc->GetNativeHandle());
    if (rtn == LLBC_OK)
    {
        LLBC_JoinThread(threadDesc->GetNativeHandle());
#if LLBC_TARGET_PLATFORM_WIN32
        if (threadDesc->GetFlags() | LLBC_ThreadFlag::Joinable)
            ::CloseHandle(threadDesc->GetNativeHandle());
#endif
        RemoveThreadDescriptor(handle);
    }

    return rtn;
}

int LLBC_ThreadManager::CancelTask(LLBC_Task *task)
{
    if (UNLIKELY(!task))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::vector<LLBC_Handle> taskThreads;
    task->GetTaskThreads(taskThreads);

    for (size_t i = 0; i < taskThreads.size(); ++i)
    {
        if (Cancel(taskThreads[i]) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_NOT_FOUND)
                continue;
            
            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::CancelGroup(LLBC_Handle handle)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadGroupDescriptor *groupDesc = 
        FindThreadGroupDescriptor(handle);
    if (!groupDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    LLBC_ThreadDescriptor *threadDesc = 
        groupDesc->FindFirstThreadDescriptor();
    while (threadDesc)
    {
        LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
        threadDesc = threadDesc->GetGroupThreadNext();

        if (Cancel(tmpThreadDesc->GetHandle()) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::CancelAll()
{
    for (int i = 0; i < LLBC_CFG_THREAD_MAX_THREAD_NUM; ++i)
    {
        LLBC_ThreadDescriptor *threadDesc = _threads[i];
        while (threadDesc)
        {
            LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
            threadDesc = threadDesc->GetThreadNext();

            if (Cancel(tmpThreadDesc->GetHandle()) != LLBC_OK)
                return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::Kill(LLBC_Handle handle, int signum)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (!threadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (threadDesc->GetState() != LLBC_ThreadState::Running &&
        threadDesc->GetState() != LLBC_ThreadState::Suspended)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    return LLBC_KillThread(threadDesc->GetNativeHandle(), signum);
}

int LLBC_ThreadManager::KillTask(LLBC_Task *task, int signum)
{
    if (UNLIKELY(!task))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    std::vector<LLBC_Handle> taskThreads;
    task->GetTaskThreads(taskThreads);

    for (size_t i = 0; i < taskThreads.size(); ++i)
    {
        if (Kill(taskThreads[i], signum) != LLBC_OK)
        {
            if (LLBC_GetLastError() == LLBC_ERROR_NOT_FOUND)
                continue;

            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::KillGroup(LLBC_Handle handle, int signum)
{
    if (UNLIKELY(handle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    LLBC_LockGuard guard(_lock);

    LLBC_ThreadGroupDescriptor *groupDesc = 
        FindThreadGroupDescriptor(handle);
    if (!groupDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    LLBC_ThreadDescriptor *threadDesc = 
        groupDesc->FindFirstThreadDescriptor();
    while (threadDesc)
    {
        LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
        threadDesc = threadDesc->GetGroupThreadNext();

        if (Kill(tmpThreadDesc->GetHandle(), signum) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadManager::KillAll(int signum)
{
    for (int i = 0; i < LLBC_CFG_THREAD_MAX_THREAD_NUM; ++i)
    {
        LLBC_ThreadDescriptor *threadDesc = _threads[i];
        while (threadDesc)
        {
            LLBC_ThreadDescriptor *tmpThreadDesc = threadDesc;
            threadDesc = threadDesc->GetThreadNext();

            if (Kill(tmpThreadDesc->GetHandle(), signum) != LLBC_OK)
                return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

void LLBC_ThreadManager::OnThreadStartup(LLBC_Handle handle)
{
    LLBC_LockGuard guard(_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    threadDesc->SetState(LLBC_ThreadState::Running);
}

void LLBC_ThreadManager::OnThreadTerminate(LLBC_Handle handle)
{
    LLBC_LockGuard guard(_lock);

    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    if (threadDesc)
    {
#if LLBC_TARGET_PLATFORM_WIN32
        if (threadDesc->GetFlags() | LLBC_ThreadFlag::Joinable)
            ::CloseHandle(threadDesc->GetNativeHandle());
#endif

        RemoveThreadDescriptor(threadDesc->GetHandle());
    }
}

LLBC_Handle LLBC_ThreadManager::CreateThread_NonLock(LLBC_ThreadProc proc,
                                                     LLBC_ThreadArg arg,
                                                     int flags,
                                                     int priority,
                                                     int stackSize,
                                                     LLBC_Task *task,
                                                     LLBC_Handle groupHandle,
                                                     LLBC_NativeThreadHandle *nativeHandle,
                                                     LLBC_Handle *handle)
{
    if (!proc)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_HANDLE;
    }
    if (stackSize <= LLBC_CFG_THREAD_MINIMUM_STACK_SIZE)
        stackSize = LLBC_CFG_THREAD_MINIMUM_STACK_SIZE;

    if (groupHandle == LLBC_INVALID_HANDLE)
        groupHandle = ++_maxGroupHandle;

    ++_maxThreadHandle;
    if (handle)
        *handle = _maxThreadHandle;

    LLBC_NativeThreadHandle tmpNativeThreadHandle = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    if (!nativeHandle)
        nativeHandle = &tmpNativeThreadHandle;

    LLBC_INTERNAL_NS __LLBC_ThreadMgr_Thread_Arg *threadArg =
        new LLBC_INTERNAL_NS __LLBC_ThreadMgr_Thread_Arg;
    threadArg->realArg = arg;
    threadArg->realProc = proc;
    threadArg->threadHandle = _maxThreadHandle;
    threadArg->threadMgr = this;

    if (LLBC_CreateThread(nativeHandle,
                          &LLBC_INTERNAL_NS __LLBC_ThreadMgr_ThreadEntry,
                          threadArg,
                          flags,
                          priority,
                          stackSize) != LLBC_OK)
    {
        delete threadArg;
        return LLBC_INVALID_HANDLE;
    }

    LLBC_ThreadDescriptor *desc = new LLBC_ThreadDescriptor;
    desc->SetHandle(_maxThreadHandle);
    desc->SetNativeHandle(*nativeHandle);
    desc->SetGroupHandle(groupHandle);
    desc->SetStackSize(stackSize);
    desc->SetPriority(priority);
    desc->SetState(LLBC_ThreadState::Stoped);
    desc->SetFlags(flags);
    desc->SetTask(task);

    AddThreadDescriptor(desc);

    return LLBC_OK;
}

void LLBC_ThreadManager::AddThreadDescriptor(LLBC_ThreadDescriptor *threadDesc)
{
    // Check thread descriptor in threads hash bucket or not.
    if (FindThreadDescriptor(threadDesc->GetHandle()))
        return;

    // Force set thread next to nullptr.
    threadDesc->SetThreadNext(nullptr);

    // Get thread group descriptor, if not exist, add new group descriptor
    // and add to thread group hash bucket.
    LLBC_ThreadGroupDescriptor *groupDesc = FindThreadGroupDescriptor(threadDesc->GetGroupHandle());
    if (!groupDesc)
    {
        groupDesc = new LLBC_ThreadGroupDescriptor;
        groupDesc->SetGroupHandle(threadDesc->GetGroupHandle());
        AddThreadGroupDescriptor(groupDesc);
    }

    // Add to thread hash bucket.
    int threadBucketIdx = threadDesc->GetHandle() % LLBC_CFG_THREAD_MAX_THREAD_NUM;
    if (!_threads[threadBucketIdx])
    {
        _threads[threadBucketIdx] = threadDesc;
    }
    else
    {
        LLBC_ThreadDescriptor *tmpDesc = _threads[threadBucketIdx];
        while (tmpDesc->GetThreadNext())
            tmpDesc = tmpDesc->GetThreadNext();

        tmpDesc->SetThreadNext(threadDesc);
    }

    // Add to thread group.
    groupDesc->AddToGroup(threadDesc);
}

LLBC_ThreadDescriptor *LLBC_ThreadManager::FindThreadDescriptor(LLBC_Handle handle) const
{
    int threadBucketIdx = handle % LLBC_CFG_THREAD_MAX_THREAD_NUM;
    if (!_threads[threadBucketIdx])
        return nullptr;

    LLBC_ThreadDescriptor *desc = _threads[threadBucketIdx];
    while (desc)
    {
        if (desc->GetHandle() == handle)
            return desc;

        desc = desc->GetThreadNext();
    }

    return nullptr;
}

void LLBC_ThreadManager::RemoveThreadDescriptor(LLBC_Handle handle)
{
    int threadBucketIdx = handle % LLBC_CFG_THREAD_MAX_THREAD_NUM;
    if (!_threads[threadBucketIdx])
        return;

    LLBC_ThreadDescriptor *desc = _threads[threadBucketIdx];
    if (desc->GetHandle() == handle)
    {
        _threads[threadBucketIdx] = desc->GetThreadNext();
        LLBC_ThreadGroupDescriptor *groupDesc = 
            FindThreadGroupDescriptor(desc->GetGroupHandle());
        groupDesc->RemoveFromGroup(desc->GetHandle());
        if (groupDesc->GetThreadCount() == 0)
            RemoveThreadGroupDescriptor(groupDesc->GetGroupHandle());

        delete desc;

        return;
    }

    while (desc->GetThreadNext())
    {
        LLBC_ThreadDescriptor *tmpDesc = desc->GetThreadNext();
        if (tmpDesc->GetHandle() == handle)
        {
            LLBC_ThreadGroupDescriptor *groupDesc =
                FindThreadGroupDescriptor(tmpDesc->GetGroupHandle());
            groupDesc->RemoveFromGroup(tmpDesc->GetHandle());
            if (groupDesc->GetThreadCount() == 0)
                RemoveThreadGroupDescriptor(groupDesc->GetGroupHandle());

            desc->SetThreadNext(desc->GetThreadNext()->GetThreadNext());

            delete tmpDesc;

            return;
        }

        desc = tmpDesc;
    }
}

void LLBC_ThreadManager::AddThreadGroupDescriptor(LLBC_ThreadGroupDescriptor *groupDesc)
{
    if (FindThreadGroupDescriptor(groupDesc->GetGroupHandle()))
        return;

    groupDesc->SetGroupNext(nullptr);

    int groupBucketIdx = groupDesc->GetGroupHandle() % LLBC_CFG_THREAD_MAX_THREAD_NUM;
    if (!_groups[groupBucketIdx])
    {
        _groups[groupBucketIdx] = groupDesc;
        return;
    }

    LLBC_ThreadGroupDescriptor *tmpGroupDesc = _groups[groupBucketIdx];
    while (tmpGroupDesc->GetGroupNext())
        tmpGroupDesc = tmpGroupDesc->GetGroupNext();

    tmpGroupDesc->SetGroupNext(groupDesc);
}

LLBC_ThreadGroupDescriptor *LLBC_ThreadManager::FindThreadGroupDescriptor(LLBC_Handle handle) const
{
    int groupBucketIdx = handle % LLBC_CFG_THREAD_MAX_THREAD_NUM;
    if (!_groups[groupBucketIdx])
        return nullptr;

    LLBC_ThreadGroupDescriptor *desc = _groups[groupBucketIdx];
    while (desc)
    {
        if (desc->GetGroupHandle() == handle)
            return desc;

        desc = desc->GetGroupNext();
    }

    return nullptr;
}

void LLBC_ThreadManager::RemoveThreadGroupDescriptor(LLBC_Handle handle)
{
    int groupBucketIdx = handle % LLBC_CFG_THREAD_MAX_THREAD_NUM;
    if (!_groups[groupBucketIdx])
        return;

    LLBC_ThreadGroupDescriptor *groupDesc = _groups[groupBucketIdx];
    if (groupDesc->GetGroupHandle() == handle)
    {
        _groups[groupBucketIdx] = groupDesc->GetGroupNext();

        delete groupDesc;

        return;
    }

    while (groupDesc->GetGroupNext())
    {
        if (groupDesc->GetGroupNext()->GetGroupHandle() == handle)
        {
            LLBC_ThreadGroupDescriptor *tmpGroupDesc = groupDesc->GetGroupNext();
            groupDesc->SetGroupNext(groupDesc->GetGroupNext()->GetGroupNext());

            delete tmpGroupDesc;

            return;
        }

        groupDesc = groupDesc->GetGroupNext();
    }
}

__LLBC_NS_END
