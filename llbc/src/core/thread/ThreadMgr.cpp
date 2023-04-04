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

#include "llbc/core/timer/TimerScheduler.h"

#include "llbc/core/objectpool/ObjectPool.h"

#include "llbc/core/objbase/AutoReleasePool.h"
#include "llbc/core/objbase/AutoReleasePoolStack.h"

#include "llbc/core/thread/Guard.h"
#include "llbc/core/thread/ThreadDescriptor.h"
#include "llbc/core/thread/ThreadGroupDescriptor.h"
#include "llbc/core/thread/ThreadMgr.h"

__LLBC_INTERNAL_NS_BEGIN

struct __LLBC_ThreadMgr_WrapThreadArg
{
    LLBC_NS LLBC_Delegate<void(void *)> entry;
    void *arg;

    LLBC_NS LLBC_ThreadMgr *threadMgr;

    LLBC_NS LLBC_Handle threadHandle;
    LLBC_NS LLBC_Handle threadGroupHandle;

    __LLBC_ThreadMgr_WrapThreadArg();
};

__LLBC_ThreadMgr_WrapThreadArg::__LLBC_ThreadMgr_WrapThreadArg()
: arg(nullptr)

, threadMgr(nullptr)

, threadHandle(LLBC_INVALID_HANDLE)
, threadGroupHandle(LLBC_INVALID_HANDLE)
{
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_ThreadMgr::LLBC_ThreadMgr()
: _maxThreadHandle(LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE)
, _maxThreadGroupHandle(LLBC_CFG_THREAD_ENTRY_THREAD_GROUP_HANDLE)
{
}

LLBC_ThreadMgr::~LLBC_ThreadMgr()
{
    WaitAll();
}

LLBC_Handle LLBC_ThreadMgr::CreateThreads(int threadNum,
                                          const LLBC_Delegate<void(void *)> &entry,
                                          void *arg,
                                          int priority,
                                          int stackSize,
                                          std::vector<LLBC_Handle> *handles,
                                          std::vector<LLBC_NativeThreadHandle> *nativeHandles)
{
    // Argument check.
    if (threadNum <= 0 ||
        !entry ||
        !LLBC_ThreadPriority::IsValid(priority))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_INVALID_HANDLE;
    }

    // Lock.
    LLBC_LockGuard guard(_lock);

    // Gen group handle.
    const LLBC_Handle groupHandle = ++_maxThreadGroupHandle;

    // Foreach to create threads.
    for (int i = 0; i < threadNum; ++i)
    {
        LLBC_NativeThreadHandle nativeHandle;
        const LLBC_Handle handle = CreateThread_NonLock(entry,
                                                        arg,
                                                        priority,
                                                        stackSize,
                                                        groupHandle,
                                                        nativeHandle);

        // TODO: impl fail rollback code.
        LLBC_ReturnIf(handle == LLBC_INVALID_HANDLE, LLBC_FAILED);

        LLBC_DoIf(handles, handles->push_back(handle));
        LLBC_DoIf(nativeHandles, nativeHandles->push_back(nativeHandle));
    }

    return groupHandle;
}

void LLBC_ThreadMgr::SetThreadStartHook(const LLBC_Delegate<void(LLBC_Handle)> &startHook)
{
    LLBC_LockGuard guard(_lock);
    _threadStartHook = startHook;
}

void LLBC_ThreadMgr::SetThreadStopHook(const LLBC_Delegate<void(LLBC_Handle)> &stopHook)
{
    LLBC_LockGuard guard(_lock);
    _threadStopHook = stopHook;
}

bool LLBC_ThreadMgr::InLLBCThread()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    return LIKELY(libTls) ? libTls->coreTls.llbcThread : false;
}

bool LLBC_ThreadMgr::InLLBCEntryThread()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    return LIKELY(libTls) ? libTls->coreTls.entryThread : false;
}

LLBC_ThreadId LLBC_ThreadMgr::CurThreadId()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls),
                           LLBC_ERROR_NOT_INIT,
                           LLBC_INVALID_NATIVE_THREAD_ID);
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls->coreTls.llbcThread),
                           LLBC_ERROR_ILLEGAL,
                           LLBC_INVALID_NATIVE_THREAD_ID);

    return libTls->coreTls.threadId;
}

LLBC_Handle LLBC_ThreadMgr::CurThreadHandle()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls),
                           LLBC_ERROR_NOT_INIT,
                           LLBC_INVALID_HANDLE);
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls->coreTls.llbcThread),
                           LLBC_ERROR_ILLEGAL,
                           LLBC_INVALID_HANDLE);

    return libTls->coreTls.threadHandle;
}

LLBC_NativeThreadHandle LLBC_ThreadMgr::CurNativeThreadHandle()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls),
                           LLBC_ERROR_NOT_INIT,
                           LLBC_INVALID_NATIVE_THREAD_HANDLE);
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls->coreTls.llbcThread),
                           LLBC_ERROR_ILLEGAL,
                           LLBC_INVALID_NATIVE_THREAD_HANDLE);

    return libTls->coreTls.nativeThreadHandle;
}

LLBC_Handle LLBC_ThreadMgr::CurGroupHandle()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls),
                           LLBC_ERROR_NOT_INIT,
                           LLBC_INVALID_HANDLE);
    LLBC_SetErrAndReturnIf(UNLIKELY(!libTls->coreTls.llbcThread),
                           LLBC_ERROR_ILLEGAL,
                           LLBC_INVALID_HANDLE);

    return libTls->coreTls.threadGroupHandle;
}

LLBC_NativeThreadHandle LLBC_ThreadMgr::GetNativeThreadHandle(LLBC_Handle handle) const
{
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           LLBC_INVALID_NATIVE_THREAD_HANDLE);
    LLBC_ReturnIf(handle == LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE,
                  __LLBC_GetEntryThreadLibTls()->coreTls.nativeThreadHandle);

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    LLBC_SetErrAndReturnIf(!threadDesc,
                           LLBC_ERROR_NOT_FOUND,
                           LLBC_INVALID_NATIVE_THREAD_HANDLE);

    return threadDesc->GetNativeHandle();
}

LLBC_Handle LLBC_ThreadMgr::GetThreadGroupHandle(LLBC_Handle handle) const
{
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           LLBC_INVALID_HANDLE);
    LLBC_ReturnIf(handle == LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE,
                  LLBC_CFG_THREAD_ENTRY_THREAD_GROUP_HANDLE);

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    LLBC_SetErrAndReturnIf(!threadDesc, LLBC_ERROR_NOT_FOUND, LLBC_INVALID_HANDLE);

    return threadDesc->GetGroupHandle();
}

int LLBC_ThreadMgr::GetPriority(LLBC_Handle handle) const
{
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           -1);
    LLBC_ReturnIf(handle == LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE,
                  LLBC_GetThreadPriority(GetNativeThreadHandle(handle)));

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    LLBC_SetErrAndReturnIf(!threadDesc, LLBC_ERROR_NOT_FOUND, -1);

    return threadDesc->GetPriority();
}

int LLBC_ThreadMgr::SetPriority(LLBC_Handle handle, int priority)
{
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           LLBC_FAILED);
    LLBC_ReturnIf(handle == LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE,
                  LLBC_SetThreadPriority(GetNativeThreadHandle(handle), priority));

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    LLBC_SetErrAndReturnIf(!threadDesc, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    if (threadDesc->GetPriority() == priority)
        return LLBC_OK;

    const int rtn = LLBC_SetThreadPriority(threadDesc->GetNativeHandle(), priority);
    if (rtn == LLBC_OK)
        threadDesc->SetPriority(priority);

    return rtn;
}

int LLBC_ThreadMgr::GetGroupThreadHandles(LLBC_Handle groupHandle, std::vector<LLBC_Handle> &handles)
{
    LLBC_SetErrAndReturnIf(UNLIKELY(groupHandle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           LLBC_FAILED);
    LLBC_DoIf(groupHandle == LLBC_CFG_THREAD_ENTRY_THREAD_GROUP_HANDLE,
              handles.push_back(LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE);
              return LLBC_OK);

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadGroupDescriptor *groupDesc = FindThreadGroupDescriptor(groupHandle);
    LLBC_SetErrAndReturnIf(!groupDesc, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    groupDesc->CollectAllThreadHandles(handles);

    return LLBC_OK;
}

int LLBC_ThreadMgr::Kill(LLBC_Handle handle, int sig)
{
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           LLBC_FAILED);
    LLBC_SetErrAndReturnIf(handle == LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE,
                           LLBC_ERROR_NOT_ALLOW,
                           LLBC_FAILED);

    LLBC_LockGuard guard(_lock);
    auto *threadDesc = FindThreadDescriptor(handle);
    LLBC_SetErrAndReturnIf(!threadDesc, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    LLBC_ReturnIf(LLBC_KillThread(
        threadDesc->GetNativeHandle(), sig) != LLBC_OK, LLBC_FAILED);

#if LLBC_TARGET_PLATFORM_WIN32
    ::CloseHandle(threadDesc->GetNativeHandle());
#endif // Win32
    RemoveThreadDescriptor(handle);

    return LLBC_OK;
}

int LLBC_ThreadMgr::KillGroup(LLBC_Handle groupHandle, int sig)
{
    std::vector<LLBC_Handle> handles;
    LLBC_ReturnIf(CollGroupOperatableHandles(groupHandle, handles) != LLBC_OK, LLBC_FAILED);

    for (size_t i = 0; i < handles.size(); ++i)
    {
        if (Kill(handles[i], sig) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadMgr::KillAll(int sig)
{
    std::vector<LLBC_Handle> handles;
    LLBC_ReturnIf(CollAllOperatableHandles(handles) != LLBC_OK, LLBC_FAILED);

    for (size_t i = 0; i < handles.size(); ++i)
    {
        if (Kill(handles[i], sig) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_ThreadMgr::ThreadEntry(void *arg)
{
    // Get wrap thread argument.
    LLBC_INL_NS __LLBC_ThreadMgr_WrapThreadArg *wrapArg = 
        reinterpret_cast<LLBC_INL_NS __LLBC_ThreadMgr_WrapThreadArg *>(arg);

    const LLBC_Handle handle = wrapArg->threadHandle;

    // Initialize lib TLS.
    LLBC_NS __LLBC_LibTls *tls = LLBC_NS __LLBC_GetLibTls();
    tls->coreTls.llbcThread = true;
    tls->coreTls.threadId = LLBC_NS LLBC_GetCurrentThreadId();
    tls->coreTls.threadHandle = handle;
    tls->coreTls.threadGroupHandle = wrapArg->threadGroupHandle;
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

     // Set thread to <Running> state.
    LLBC_ThreadMgr * const threadMgr = wrapArg->threadMgr;
    threadMgr->_lock.Lock();
    LLBC_ThreadDescriptor *threadDesc = threadMgr->FindThreadDescriptor(handle);
    threadDesc->SetState(LLBC_ThreadState::Running);

    const LLBC_Delegate<void(LLBC_Handle)> startHook = threadMgr->_threadStartHook;
    threadMgr->_lock.Unlock();

    // Invoke thread start hook.
    LLBC_DoIf(startHook, startHook(handle));


    // Call thread proc.
    // ==========================================
    wrapArg->entry(wrapArg->arg);
    // ==========================================

    // Invoke thread stop hook.
    threadMgr->_lock.Lock();
    LLBC_Delegate<void(LLBC_Handle)> stopHook = threadMgr->_threadStopHook;
    threadMgr->_lock.Unlock();

    LLBC_DoIf(stopHook, stopHook(handle));

    // Remove thread descriptor.
    threadMgr->_lock.Lock();
    threadDesc = threadMgr->FindThreadDescriptor(handle);
    if (LIKELY(threadDesc))
    {
#if LLBC_TARGET_PLATFORM_WIN32
        ::CloseHandle(threadDesc->GetNativeHandle());
#endif

        threadMgr->RemoveThreadDescriptor(threadDesc->GetHandle());
    }
    threadMgr->_lock.Unlock();

    // Delete arg.
    delete wrapArg;

    // Cleanup objbase tls components.
    delete reinterpret_cast<LLBC_NS LLBC_AutoReleasePoolStack *>(
        tls->objbaseTls.poolStack); tls->objbaseTls.poolStack = nullptr;
    // Cleanup core tls components.
    delete reinterpret_cast<LLBC_NS LLBC_TimerScheduler *>(
        tls->coreTls.timerScheduler); tls->coreTls.timerScheduler = nullptr;
    delete reinterpret_cast<LLBC_NS LLBC_SafetyObjectPool *>(
        tls->coreTls.safetyObjectPool); tls->coreTls.safetyObjectPool = nullptr;
    delete reinterpret_cast<LLBC_NS LLBC_UnsafetyObjectPool *>(
        tls->coreTls.unsafetyObjectPool); tls->coreTls.unsafetyObjectPool = nullptr;
#if LLBC_TARGET_PLATFORM_WIN32
    ::CloseHandle(tls->coreTls.nativeThreadHandle);
#endif // LLBC_TARGET_PLATFORM_WIN32
    LLBC_NS __LLBC_ResetLibTls();
}

LLBC_Handle LLBC_ThreadMgr::CreateThread_NonLock(const LLBC_Delegate<void(void *)> &entry,
                                                 void *arg,
                                                 int priority,
                                                 int stackSize,
                                                 LLBC_Handle groupHandle,
                                                 LLBC_NativeThreadHandle &nativeHandle)
{
    // Gen thread handle.
    const LLBC_Handle handle = ++_maxThreadHandle;

    // Create wrap thread argument.
    LLBC_INL_NS __LLBC_ThreadMgr_WrapThreadArg *wrapArg =
            new LLBC_INL_NS __LLBC_ThreadMgr_WrapThreadArg;
    wrapArg->entry = entry;
    wrapArg->arg = arg;
    wrapArg->threadMgr = this;
    wrapArg->threadHandle = handle;
    wrapArg->threadGroupHandle = groupHandle;

    // Create thread.
    if (LLBC_CreateThread(&LLBC_ThreadMgr::ThreadEntry,
                          wrapArg,
                          &nativeHandle,
                          priority,
                          stackSize) != LLBC_OK)
    {
        delete wrapArg;
        return LLBC_INVALID_HANDLE;
    }

    // Create thread descriptor.
    LLBC_ThreadDescriptor *desc = new LLBC_ThreadDescriptor;
    desc->SetHandle(handle);
    desc->SetNativeHandle(nativeHandle);
    desc->SetGroupHandle(groupHandle);
    desc->SetPriority(priority);
    desc->SetState(LLBC_ThreadState::Stoped);

    // Add thread descriptor to thread manager.
    AddThreadDescriptor(desc);

    return LLBC_OK;
}

void LLBC_ThreadMgr::AddThreadDescriptor(LLBC_ThreadDescriptor *threadDesc)
{
    // Get thread group descriptor, if not exist, add new group descriptor.
    // and add to thread group hash bucket.
    LLBC_ThreadGroupDescriptor *groupDesc = FindThreadGroupDescriptor(threadDesc->GetGroupHandle());
    if (!groupDesc)
    {
        groupDesc = new LLBC_ThreadGroupDescriptor;
        groupDesc->SetGroupHandle(threadDesc->GetGroupHandle());

        AddThreadGroupDescriptor(groupDesc);
    }

    // Add to thread hash bucket.
    _hldr2Threads.emplace(threadDesc->GetHandle(), threadDesc);
    _nativeHldr2Threads.emplace(threadDesc->GetNativeHandle(), threadDesc);

    // Add to thread group.
    groupDesc->AddToGroup(threadDesc);
}

LLBC_ThreadDescriptor *LLBC_ThreadMgr::FindThreadDescriptor(LLBC_Handle handle) const
{
    const auto it = _hldr2Threads.find(handle);
    return it != _hldr2Threads.end() ? it->second : nullptr;
}

void LLBC_ThreadMgr::RemoveThreadDescriptor(LLBC_Handle handle)
{
    const auto it = _hldr2Threads.find(handle);
    LLBC_ReturnIf(it == _hldr2Threads.end(), void());

    // Remove from thread group.
    LLBC_ThreadDescriptor *threadDesc = it->second;
    LLBC_ThreadGroupDescriptor *groupDesc = FindThreadGroupDescriptor(threadDesc->GetGroupHandle());
    groupDesc->RemoveFromGroup(handle);
    LLBC_DoIf(groupDesc->GetThreadsCount() == 0,
              RemoveThreadGroupDescriptor(groupDesc->GetGroupHandle()));

    // Remove from collections.
    _nativeHldr2Threads.erase(threadDesc->GetNativeHandle());
    _hldr2Threads.erase(it);

    // Delete thread descriptor.
    delete threadDesc;
}

void LLBC_ThreadMgr::AddThreadGroupDescriptor(LLBC_ThreadGroupDescriptor *groupDesc)
{
    _hldr2Groups.emplace(groupDesc->GetGroupHandle(), groupDesc);
}

LLBC_ThreadGroupDescriptor *LLBC_ThreadMgr::FindThreadGroupDescriptor(LLBC_Handle groupHandle) const
{
    const auto it = _hldr2Groups.find(groupHandle);
    return it != _hldr2Groups.end() ? it->second : nullptr;
}

void LLBC_ThreadMgr::RemoveThreadGroupDescriptor(LLBC_Handle groupHandle)
{
    const auto groupIt = _hldr2Groups.find(groupHandle);
    LLBC_ReturnIf(groupIt == _hldr2Groups.end(), void());

    // Remove all thread descriptors.
    LLBC_ThreadGroupDescriptor *groupDesc = groupIt->second;
    auto &threadDescs = groupDesc->GetAllThreadDescriptors();
    for (auto threadIt = threadDescs.begin(); threadIt != threadDescs.end(); )
        RemoveThreadDescriptor(threadIt++->first);

    // Remove group descriptor from collection.
    _hldr2Groups.erase(groupIt);

    // Delete group descriptor.
    delete groupDesc;
}

int LLBC_ThreadMgr::WaitOrCancelThread(LLBC_Handle handle, bool wait)
{
    // Get operatable native thread handle.
    const LLBC_NativeThreadHandle nativeHandle = GetOperatableNativeHandle(handle);
    LLBC_ReturnIf(nativeHandle == LLBC_INVALID_NATIVE_THREAD_HANDLE, LLBC_FAILED);

    // Join/Cancel thread.
    const int rtn = wait ?  LLBC_JoinThread(nativeHandle) : LLBC_CancelThread(nativeHandle);
    if (rtn == LLBC_OK)
    {
        LLBC_LockGuard guard(_lock);
        LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
        if (threadDesc)
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            ::CloseHandle(threadDesc->GetNativeHandle());
            #endif // Win32

            RemoveThreadDescriptor(handle);
        }
    }

    return rtn;
}

int LLBC_ThreadMgr::WaitOrCancelGroup(LLBC_Handle groupHandle, bool wait)
{
    // Collect operatable thread handles in given thread group.
    std::vector<LLBC_Handle> handles;
    LLBC_ReturnIf(CollGroupOperatableHandles(groupHandle, handles) != LLBC_OK, LLBC_FAILED);

    // Wait/Cancel group threads.
    for (size_t i = 0; i < handles.size(); ++i)
    {
        if (WaitOrCancelThread(handles[i], wait) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadMgr::WaitOrCancelAll(bool wait)
{
    // Collect all thread handles.
    std::vector<LLBC_Handle> handles;
    LLBC_ReturnIf(CollAllOperatableHandles(handles) != LLBC_OK, LLBC_FAILED);

    // Wait/Cancel threads.
    for (size_t i = 0; i < handles.size(); ++i)
    {
        if (WaitOrCancelThread(handles[i], wait) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadMgr::SuspendOrResumeThread(LLBC_Handle handle, bool suspend)
{
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           LLBC_FAILED);
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE),
                           LLBC_ERROR_NOT_ALLOW,
                           LLBC_FAILED);

    const int toThreadState =
        suspend ? LLBC_ThreadState::Suspended : LLBC_ThreadState::Running;

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    LLBC_SetErrAndReturnIf(!threadDesc, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);
    LLBC_SetErrAndReturnIf(threadDesc->GetState() == toThreadState,
                           LLBC_ERROR_REENTRY,
                           LLBC_FAILED);
    LLBC_SetErrAndReturnIf(threadDesc->GetState() == LLBC_ThreadState::Stoped,
                           LLBC_ERROR_NOT_INIT,
                           LLBC_FAILED);

    const LLBC_NativeThreadHandle nativeThreadHandle = threadDesc->GetNativeHandle();
    const int rtn = suspend ?
        LLBC_SuspendThread(nativeThreadHandle) : LLBC_ResumeThread(nativeThreadHandle);
    if (rtn == LLBC_OK)
        threadDesc->SetState(toThreadState);

    return rtn;
}

int LLBC_ThreadMgr::SuspendOrResumeGroup(LLBC_Handle groupHandle, bool suspend)
{
    std::vector<LLBC_Handle> handles;
    LLBC_ReturnIf(CollGroupOperatableHandles(groupHandle, handles) != LLBC_OK, LLBC_FAILED);

    for (size_t i = 0; i < handles.size(); ++i)
    {
        if (SuspendOrResumeThread(handles[i], suspend) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_ThreadMgr::SuspendOrResumeAll(bool suspend)
{
    std::vector<LLBC_Handle> handles;
    LLBC_ReturnIf(CollAllOperatableHandles(handles) != LLBC_OK, LLBC_FAILED);

    for (size_t i = 0; i < handles.size(); ++i)
    {
        if (SuspendOrResumeThread(handles[i], suspend) != LLBC_OK &&
            LLBC_GetLastError() != LLBC_ERROR_NOT_FOUND)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

LLBC_NativeThreadHandle LLBC_ThreadMgr::GetOperatableNativeHandle(LLBC_Handle handle)
{
    LLBC_SetErrAndReturnIf(UNLIKELY(handle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_INVALID,
                           LLBC_INVALID_NATIVE_THREAD_HANDLE);
    LLBC_SetErrAndReturnIf(handle == LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE ||
                               handle == __LLBC_GetLibTls()->coreTls.threadHandle,
                           LLBC_ERROR_NOT_ALLOW,
                           LLBC_INVALID_NATIVE_THREAD_HANDLE);

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadDescriptor *threadDesc = FindThreadDescriptor(handle);
    LLBC_SetErrAndReturnIf(!threadDesc,
                           LLBC_ERROR_NOT_FOUND,
                           LLBC_INVALID_NATIVE_THREAD_HANDLE);

    return threadDesc->GetNativeHandle();
}

int LLBC_ThreadMgr::CollGroupOperatableHandles(LLBC_Handle groupHandle,
                                                   std::vector<LLBC_Handle> &handles)
{
    LLBC_SetErrAndReturnIf(UNLIKELY(groupHandle == LLBC_INVALID_HANDLE),
                           LLBC_ERROR_ARG,
                           LLBC_FAILED);
    LLBC_SetErrAndReturnIf(groupHandle == LLBC_CFG_THREAD_ENTRY_THREAD_GROUP_HANDLE ||
                               groupHandle == __LLBC_GetLibTls()->coreTls.threadGroupHandle,
                           LLBC_ERROR_NOT_ALLOW,
                           LLBC_FAILED);

    LLBC_LockGuard guard(_lock);
    LLBC_ThreadGroupDescriptor *groupDesc = FindThreadGroupDescriptor(groupHandle);
    LLBC_SetErrAndReturnIf(!groupDesc, LLBC_ERROR_NOT_FOUND, LLBC_FAILED);

    groupDesc->CollectAllThreadHandles(handles);

    return LLBC_OK;
}

int LLBC_ThreadMgr::CollAllOperatableHandles(std::vector<LLBC_Handle> &handles) const
{
    LLBC_SetErrAndReturnIf(!__LLBC_GetLibTls()->coreTls.entryThread,
                           LLBC_ERROR_NOT_ALLOW,
                           LLBC_FAILED);

    LLBC_LockGuard guard(_lock);
    handles.reserve(handles.size() + _hldr2Threads.size());
    LLBC_Foreach(_hldr2Threads, handles.emplace_back(item.first));

    return LLBC_OK;
}

__LLBC_NS_END
