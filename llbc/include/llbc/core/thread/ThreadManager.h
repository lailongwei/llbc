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

#include "llbc/common/Common.h"

#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/singleton/Singleton.h"

__LLBC_NS_BEGIN
class LLBC_Task;
class LLBC_ThreadDescriptor;
class LLBC_ThreadGroupDescriptor;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief LLBC library thread manager class encapsulation.
 */
class LLBC_EXPORT LLBC_ThreadManager
{
public:
    LLBC_ThreadManager();
    ~LLBC_ThreadManager();

public:
    /**
     * Create threads.
     * @param[in] threadNum       - thread number, must >= 1.
     * @param[in] proc            - thread routine.
     * @param[in] arg             - thread routine argument, default is nullptr.
     * @param[in] flags           - thread flags, default is Joinable.
     * @param[in] priority        - thread priority, default is Normal.
     * @param[in] task            - task pointer.
     * @param[in/out] groupHandle - group handle, if is INVALID_HANDLE, will auto set new group handle.
     * @param[out] nativeHandles  - all native thread handles will store here, generate by OS.
     * @param[out] handles        - all thread handles will store here, generate by thread manager.
     * @return LLBC_Handle - return thread group handle if success, otherwise return LLBC_INVALID_HANDLE.
     */
    LLBC_Handle CreateThreads(int threadNum,
                              LLBC_ThreadProc proc,
                              LLBC_ThreadArg arg = nullptr,
                              int flags = LLBC_ThreadFlag::Joinable,
                              int priority = LLBC_ThreadPriority::Normal,
                              const int stackSize[] = nullptr,
                              LLBC_Task *task = nullptr,
                              LLBC_Handle groupHandle = LLBC_INVALID_HANDLE,
                              LLBC_NativeThreadHandle nativeHandles[] = nullptr,
                              LLBC_Handle handles[] = nullptr);

    /**
     * Create thread.
     * @param[in] proc          - thread routine.
     * @param[in] arg           - thread argument, default is nullptr.
     * @param[in] flags         - thread flags, default is Joinable.
     * @param[in] priority      - thread priority, default is Normal.
     * @param[in] stackSize     - default stack size, default is DFT_SIZE.
     * @param[in] groupHandle   - group handle, if is INVALID_HANDLE, will auto set new group handle.
     * @param[out] nativeHandle - native thread handle, generate by OS.
     * @param[out] handle       - thread handle, generate by thread manager.
     * @return LLBC_Handle - return thread group handle if success, otherwise return LLBC_INVALID_HANDLE.
     */
    LLBC_Handle CreateThread(LLBC_ThreadProc proc,
                             LLBC_ThreadArg arg = nullptr,
                             int flags = LLBC_ThreadFlag::Joinable,
                             int priority = LLBC_ThreadPriority::Normal,
                             int stackSize = LLBC_CFG_THREAD_DFT_STACK_SIZE,
                             LLBC_Task *task = nullptr,
                             LLBC_Handle groupHandle = LLBC_INVALID_HANDLE,
                             LLBC_NativeThreadHandle *nativeHandle = nullptr,
                             LLBC_Handle *handle = nullptr);

public:
    /**
     * Static function, let thread sleep specific times.
     * @param[in] milliSecs - milli seconds.
     */
    static void Sleep(int milliSecs);

    /**
     * Relax the CPU.
     */
    static void CPURelax();

    /**
     * Get current thread native thread handle.
     * @return LLBC_NativeThreadHandle - native thread handle.
     */
    static LLBC_NativeThreadHandle GetCurrentThread();

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
    LLBC_Handle GetGroupHandle(LLBC_Handle handle) const;

    /**
     * Get thread task.
     * @param[in] handle - thread handle.
     * @return LLBC_Task * - thread task.
     */
    LLBC_Task *GetTask(LLBC_Handle handle) const;

    /**
     * Get thread stack size.
     * @param[in] handle - thread handle.
     * @return int - thread stack size, if return -1. it means error occurred.
     */
    int GetStackSize(LLBC_Handle handle) const;

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
     * @return int - return 0 if successed, otherwise return -1.
     */
    int SetPriority(LLBC_Handle handle, int priority);

public:
    /**
     * Wait specific thread to terminate.
     * @param[in] handle - thread handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int Wait(LLBC_Handle handle);

    /**
     * Wait specific task to terminate.
     * @param[in] task - thread task.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int WaitTask(LLBC_Task *task);

    /**
     * Wait specific thread group to terminate.
     * @param[in] handle - group handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int WaitGroup(LLBC_Handle handle);

    /**
     * Wait all threads(Not include main thread) to terminate.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int WaitAll();

public:
    /**
     * Suspend thread.
     * @param[in] handle - thread handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int Suspend(LLBC_Handle handle);

    /**
     * Suspend task.
     * @param[in] task - thread task.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int SuspendTask(LLBC_Task *task);

    /**
     * Suspend thread group.
     * @param[in] handle - thread group handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int SuspendGroup(LLBC_Handle handle);

    /**
     * Suspend all threads(Not include main thread).
     * @return int - return 0 if successed, otherwise return -1.
     */
    int SuspendAll();

public:
    /**
     * Resume thread.
     * @param[in] handle - thread handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int Resume(LLBC_Handle handle);

    /**
     * Resume task.
     * @param[in] task - thread task.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int ResumeTask(LLBC_Task *task);

    /**
     * Resume thread group.
     * @param[in] handle - thread group handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int ResumeGroup(LLBC_Handle handle);

    /**
     * Resume all threads(Not include main thread).
     * @return int - return 0 if successed, otherwise return -1.
     */
    int ResumeAll();

public:
    /**
     * Cancel thread.
     * @param[in] handle - thread handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int Cancel(LLBC_Handle handle);

    /**
     * Cancel thread task.
     * @param[in] task - thread task.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int CancelTask(LLBC_Task *task);

    /**
     * Cancel thread group.
     * @param[in] handle - thread group handle.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int CancelGroup(LLBC_Handle handle);

    /**
     * Cancel all threads(not include main thread).
     * @return int - return 0 if successed, otherwise return -1.
     */
    int CancelAll();

public:
    /**
     * Send signal to specific thread.
     * @param[in] handle - thread handle.
     * @param[in] signum - signal number.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int Kill(LLBC_Handle handle, int signum);

    /**
     * Send signal to specific task.
     * @param[in] task   - thread task.
     * @parampin[ signum - signal number.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int KillTask(LLBC_Task *task, int signum);

    /**
     * Send signal to specific thread group.
     * @param[in] handle - thread group handle.
     * @param[in[ signum - signal number.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int KillGroup(LLBC_Handle handle, int signum);

    /**
     * Send signal to all threads.
     * @param[in] signum - signal number.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int KillAll(int signum);

public:
    /**
     * Thread startup event handle function, call by thread entry function.
     * @param[in] handle - thread handle.
     */
    void OnThreadStartup(LLBC_Handle handle);

    /**
     * Thread terminate event handle function, call by Thread entry function.
     * @param[in] handle - thread handle.
     */
    void OnThreadTerminate(LLBC_Handle handle);

private:
    /**
     * Create thread(non lock).
     * @param[in] proc          - thread routine.
     * @param[in] arg           - thread argument, default is nullptr.
     * @param[in] flags         - thread flags, default is Joinable.
     * @param[in] priority      - thread priority, default is Normal.
     * @param[in] stackSize     - default stack size, default is DFT_SIZE.
     * @param[in] groupHandle   - group handle, if is INVALID_HANDLE, will auto set new group handle.
     * @param[out] nativeHandle - native thread handle, generate by OS.
     * @param[out] handle       - thread handle, generate by thread manager.
     * @return LLBC_Handle - return thread group handle if success, otherwise return LLBC_INVALID_HANDLE.
     */
    LLBC_Handle CreateThread_NonLock(LLBC_ThreadProc proc,
                                     LLBC_ThreadArg arg = nullptr,
                                     int flags = LLBC_ThreadFlag::Joinable,
                                     int priority = LLBC_ThreadPriority::Normal,
                                     int stackSize = LLBC_CFG_THREAD_DFT_STACK_SIZE,
                                     LLBC_Task *task = nullptr,
                                     LLBC_Handle groupHandle = LLBC_INVALID_HANDLE,
                                     LLBC_NativeThreadHandle *nativeHandle = nullptr,
                                     LLBC_Handle *handle = nullptr);

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
     * Find specific thread group descriptor by given thread group handle.
     * @param[in] handle - thread group handle.
     * @return LLBC_ThreadGroupDescriptor * - thread group descriptor.
     */
    LLBC_ThreadGroupDescriptor *FindThreadGroupDescriptor(LLBC_Handle handle) const;

    /**
     * Remove thread group descriptor from thread manager.
     * @param[in] handle - thread group handle.
     */
    void RemoveThreadGroupDescriptor(LLBC_Handle handle);

private:
    LLBC_Handle _maxThreadHandle;
    LLBC_Handle _maxGroupHandle;

    LLBC_ThreadDescriptor *_threads[LLBC_CFG_THREAD_MAX_THREAD_NUM];
    LLBC_ThreadGroupDescriptor *_groups[LLBC_CFG_THREAD_MAX_THREAD_NUM];

    LLBC_SpinLock _lock;
};

/**
 * Singleton macro define.
 */
template class LLBC_EXPORT LLBC_Singleton<LLBC_ThreadManager>;
#define LLBC_ThreadManagerSingleton LLBC_Singleton<LLBC_NS LLBC_ThreadManager>::Instance()

__LLBC_NS_END

#include "llbc/core/thread/ThreadManagerInl.h"


