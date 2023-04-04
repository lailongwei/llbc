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

#pragma once

#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/utils/Util_Delegate.h"
#include "llbc/core/singleton/Singleton.h"

#include "llbc/core/thread/SpinLock.h"

__LLBC_NS_BEGIN
class LLBC_Task;
class LLBC_ThreadDescriptor;
class LLBC_ThreadGroupDescriptor;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief LLBC library thread manager class encapsulation.
 */
class LLBC_EXPORT LLBC_ThreadMgr final
{
public:
    LLBC_ThreadMgr();
    ~LLBC_ThreadMgr();

public:
    /**
     * Create threads.
     * @param[in] threadNum      - thread number, must >= 1.
     * @param[in] entry          - thread entry.
     * @param[in] arg            - thread argument.
     * @param[in] priority       - thread priority, default is Normal.
     * @param[in] stackSize      - stack size, in bytes.
     * @param[out] handles       - all thread handles will store here, generate by thread manager.
     * @param[out] nativeHandles - all native thread handles will store here, generate by OS.
     * @return LLBC_Handle - return thread group handle if success, otherwise return LLBC_INVALID_HANDLE.
     */
    LLBC_Handle CreateThreads(int threadNum,
                              const LLBC_Delegate<void(void *)> &entry,
                              void *arg = nullptr,
                              int priority = LLBC_ThreadPriority::Normal,
                              int stackSize = 0,
                              std::vector<LLBC_Handle> *handles = nullptr,
                              std::vector<LLBC_NativeThreadHandle> *nativeHandles = nullptr);

    /**
     * Set thread start hook.
     * @param[in] startHook - thread start hook.
     */
    void SetThreadStartHook(const LLBC_Delegate<void(LLBC_Handle)> &startHook);

    /**
     * Set thread stop hook.
     * @param[in] stopHook - thread stop hook.
     */
    void SetThreadStopHook(const LLBC_Delegate<void(LLBC_Handle)> &stopHook);

public:
    /**
     * Check in llbc thread or not.
     * @reutrn bool - return llbc thread flag.
     */
    static bool InLLBCThread();

    /**
     * Check in llbc entry thread or not.
     * @return bool - return llbc entry thread flag.
     */
    static bool InLLBCEntryThread();

    /**
     * Get current threadId.
     * @return LLBC_ThreadId - current thread id.
     */
    static LLBC_ThreadId CurThreadId();

    /**
     * Get current thread handle.
     * @return LLBC_Handle - current thread handle.
     */
    static LLBC_Handle CurThreadHandle();

    /**
     * Get current native thread handle.
     * @return LLBC_NativeThreadHandle - native thread handle.
     *                                   Note: In win32 platform, don't need call CloseHandle().
     */
    static LLBC_NativeThreadHandle CurNativeThreadHandle();

    /**
     * Get current thread group handle.
     * @return LLBC_Handle - current thread group handle.
     */
    static LLBC_Handle CurGroupHandle();

public:
    /**
     * Get native thread handle.
     * @param[in] handle - thread handle.
     * @return LLBC_NativeThreadHandle - native thread handle.
     */
    LLBC_NativeThreadHandle GetNativeThreadHandle(LLBC_Handle handle) const;

    /**
     * Get thread group handle.
     * @param[in] handle - thread handle.
     * @return LLBC_Handle - thread group handle.
     */
    LLBC_Handle GetThreadGroupHandle(LLBC_Handle handle) const;

    /**
     * Get thread priority.
     * @param[in] handle - thread handle.
     * @return int - thread priority, if return -1, it means error occurred.
     */
    int GetPriority(LLBC_Handle handle) const;

    /**
     * Set thread priority.
     * @param[in] handle   - thread handle.
     * @param[in] priority - thread priority.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetPriority(LLBC_Handle handle, int priority);

    /**
     * Get running group thread handles.
     * @param[in] groupHandle - thread group handle.
     * @param[out] handles    - the group thread handles.
     * @return int - return 0 if success, otherwise return -1.
     */
    int GetGroupThreadHandles(LLBC_Handle groupHandle, std::vector<LLBC_Handle> &handles);

public:
    /**
     * Wait specific thread.
     * @param[in] handle - thread handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Wait(LLBC_Handle handle);

    /**
     * Wait specific thread group.
     * @param[in] groupHandle - thread group handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int WaitGroup(LLBC_Handle groupHandle);

    /**
     * Wait all threads(Not include main thread).
     * @return int - return 0 if success, otherwise return -1.
     */
    int WaitAll();

public:
    /**
     * Cancel thread.
     * @param[in] handle - thread handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Cancel(LLBC_Handle handle);

    /**
     * Cancel thread group.
     * @param[in] groupHandle - thread group handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CancelGroup(LLBC_Handle groupHandle);

    /**
     * Cancel all threads(not include main thread).
     * @return int - return 0 if success, otherwise return -1.
     */
    int CancelAll();

public:
    /**
     * Suspend thread.
     * @param[in] handle - thread handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Suspend(LLBC_Handle handle);

    /**
     * Suspend thread group.
     * @param[in] groupHandle - thread group handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SuspendGroup(LLBC_Handle groupHandle);

    /**
     * Suspend all threads(Not include main thread).
     * @return int - return 0 if success, otherwise return -1.
     */
    int SuspendAll();

public:
    /**
     * Resume thread.
     * @param[in] handle - thread handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Resume(LLBC_Handle handle);

    /**
     * Resume thread group.
     * @param[in] groupHandle - thread group handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int ResumeGroup(LLBC_Handle groupHandle);

    /**
     * Resume all threads(Not include main thread).
     * @return int - return 0 if success, otherwise return -1.
     */
    int ResumeAll();

public:
    /**
     * Send signal to specific thread.
     * @param[in] handle - thread handle.
     * @param[in] sig - signal number.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Kill(LLBC_Handle handle, int sig);

    /**
     * Send signal to specific thread group.
     * @param[in] groupHandle - thread group handle.
     * @param[in[ sig - signal number.
     * @return int - return 0 if success, otherwise return -1.
     */
    int KillGroup(LLBC_Handle groupHandle, int sig);

    /**
     * Send signal to all threads.
     * @param[in] sig - signal number.
     * @return int - return 0 if success, otherwise return -1.
     */
    int KillAll(int sig);

private:
    /**
     * llbc library thread entry.
     * @param[in] arg - thread arg.
     */
    static void ThreadEntry(void *arg);

private:
    /**
     * Create thread(non lock).
     * @param[in] entry         - thread entry.
     * @param[in] arg           - thread argument.
     * @param[in] priority      - thread priority.
     * @param[in] stackSize     - default stack size.
     * @param[in] groupHandle   - group handle.
     * @param[out] nativeHandle - native thread handle, generate by OS.
     * @return LLBC_Handle - return thread handle if success, otherwise return LLBC_INVALID_HANDLE.
     */
    LLBC_Handle CreateThread_NonLock(const LLBC_Delegate<void(void *)> &entry,
                                     void *arg,
                                     int priority,
                                     int stackSize,
                                     LLBC_Handle groupHandle,
                                     LLBC_NativeThreadHandle &nativeHandle);

private:
    /**
     * Add new thread descriptor to the thread manager.
     * @param[in] threadDesc - thread descriptor.
     */
    void AddThreadDescriptor(LLBC_ThreadDescriptor *threadDesc);

    /**
     * Find specific thread descriptor by given thread handle.
     * @param[in] handle - thread handle.
     * @return LLBC_ThreadDescriptor * - thread descriptor.
     */
    LLBC_ThreadDescriptor *FindThreadDescriptor(LLBC_Handle handle) const;

    /**
     * Remove the thread descriptor.
     * @param[in] handle - thread handle.
     */
    void RemoveThreadDescriptor(LLBC_Handle handle);

    /**
     * Add new thread group descriptor to the thread manager.
     * @param[in] groupDesc - thread group descriptor.
     */
    void AddThreadGroupDescriptor(LLBC_ThreadGroupDescriptor *groupDesc);

    /**
     * Find specific thread group descriptor by given thread group groupHandle.
     * @param[in] groupHandle - thread group handle.
     * @return LLBC_ThreadGroupDescriptor * - thread group descriptor.
     */
    LLBC_ThreadGroupDescriptor *FindThreadGroupDescriptor(LLBC_Handle groupHandle) const;

    /**
     * Remove thread group descriptor from thread manager.
     * @param[in] groupHandle - thread group handle.
     */
    void RemoveThreadGroupDescriptor(LLBC_Handle groupHandle);

private:
    /**
     * Wait or cancel thread.
     * @param[in] handle - thread handle.
     * @param[in] wait   - wait flag, true execute wait operation, otherwise execute cancel operation.
     * @return int - return 0 if success, otherwise return -1.
     */
    int WaitOrCancelThread(LLBC_Handle handle, bool wait);

    /**
     * Wait or cancel thread group.
     * @param[in] groupHandle - thread group handle.
     * @param[in] wait        - wait flag, true execute wait operation, otherwise execute cancel operation.
     * @return int - return 0 if success, otherwise return -1.
     */
    int WaitOrCancelGroup(LLBC_Handle groupHandle, bool wait);

    /**
     * Wait or cancel all threads.
     * @param[in] wait - wait flag, true execute wait operation, otherwise execute cancel operation.
     * @return int - return 0 if success, otherwise return -1.
     */
    int WaitOrCancelAll(bool wait);

    /**
     * Suspend or resume thread.
     * @param[in] handle  - thread handle.
     * @param[in] suspend - suspend flag, true execute suspend operation, otherwise execute resume operation.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SuspendOrResumeThread(LLBC_Handle handle, bool suspend);

    /**
     * Suspend or resume thread group.
     * @param[in] groupHandle - thread group handle.
     * @param[in] suspend     - suspend flag, true execute suspend operation, otherwise execute resume operation.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SuspendOrResumeGroup(LLBC_Handle groupHandle, bool suspend);

    /**
     * Suspend or resume all threads.
     * @param[in] suspend - suspend flag, true execute suspend operation, otherwise execute resume operation.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SuspendOrResumeAll(bool suspend);

private:
    /**
     * Get operatable(Cancel/Wait/Suspend/Resume/Kill) native thread handle.
     * @param[in] handle - thread handle.
     * @return LLBC_NativeThreadHandle - the native thread handle.
     */
    LLBC_NativeThreadHandle GetOperatableNativeHandle(LLBC_Handle handle);

    /**
     * Collect group operable thread handles.
     * @param[in] groupHandle - the thread group handle.
     * @param[out] handles    - the thread handles.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CollGroupOperatableHandles(LLBC_Handle groupHandle,
                                      std::vector<LLBC_Handle> &handles);

    /**
     * Collect all operatable thread handles.
     * @param[in] handles - the thread handles.
     * @return int - return 0 if success, otherwise return -1.
     */
    int CollAllOperatableHandles(std::vector<LLBC_Handle> &handles) const;

private:
    mutable LLBC_SpinLock _lock;

    volatile LLBC_Handle _maxThreadHandle;
    volatile LLBC_Handle _maxThreadGroupHandle;

    LLBC_Delegate<void(LLBC_Handle)> _threadStartHook;
    LLBC_Delegate<void(LLBC_Handle)> _threadStopHook;

    std::unordered_map<LLBC_Handle, LLBC_ThreadDescriptor *> _hldr2Threads;
    std::unordered_map<LLBC_NativeThreadHandle, LLBC_ThreadDescriptor *> _nativeHldr2Threads;
    std::unordered_map<LLBC_Handle, LLBC_ThreadGroupDescriptor *> _hldr2Groups;
};

/**
 * Singleton macro define.
 */
template class LLBC_EXPORT LLBC_Singleton<LLBC_ThreadMgr>;
#define LLBC_ThreadMgrSingleton LLBC_Singleton<LLBC_NS LLBC_ThreadMgr>::Instance()

__LLBC_NS_END

#include "llbc/core/thread/ThreadMgrInl.h"


