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
#include "llbc/core/thread/SpinLock.h"

#include "llbc/core/thread/TaskQueue.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_ThreadMgr;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The task state enumeration.
 */
class LLBC_EXPORT LLBC_TaskState
{
public:
    enum
    {
        Begin,
        NotActivated = Begin,
        Activating,
        Activated,
        Deactivating,

        End
    };

    /**
     * Get task state describe.
     * @param[in] taskState - the task state.
     * @return const char * - the task state describe.
     */
    static const char *GetDesc(int taskState);
};

/**
 * \brief Task class encapsulation.
 */
template<typename Queue>
class LLBC_TaskBase : public Queue
{
public:
    LLBC_TaskBase(LLBC_ThreadMgr *threadMgr = nullptr);
    virtual ~LLBC_TaskBase();

public:
    /**
     * Activate current task.
     * @param[in] threadNum         - thread number.
     * @param[in] threadPriority    - thread priority.
     * @param[in] stackSize         - per thread stack size, in bytes.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Activate(int threadNum = 1,
                         int threadPriority = LLBC_ThreadPriority::Normal,
                         int stackSize = LLBC_CFG_THREAD_DFT_STACK_SIZE);

    /**
     * Check task is activated or not.
     * @return bool - the activated flag.
     */
    bool IsActivated() const;

    /**
     * Get task state.
     * @return int - the task state enumeration.
     */
    int GetTaskState() const;

    /**
     * Get thread manager.
     * @return LLBC_ThreadMgr * - thread manager.
     */
    LLBC_ThreadMgr *GetThreadMgr() const;

    /**
     * Get task thread group handle.
     * @return LLBC_Handle - task thread group handle.
     */
    LLBC_Handle GetThreadGroupHandle() const;

public:
    /**
     * Wait current task.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Wait();

public:
    /**
     * Service method, application should expand it.
     */
    virtual void Svc() = 0;

    /**
     * Cleanup method, when all threads terminated, will call this method to cleanup task.
     */
    virtual void Cleanup() = 0;

    /**
     * Get current thread processor id.
     * @return int - current thread processor id.
     */
    static int GetProcessorId();

private:
    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_TaskBase);

    /*
     * Task internal cleanup.
     */
    void InternalCleanup();

    /**
     * Task entry method
     */
    void TaskEntry(void *arg);

private:
    LLBC_SpinLock _lock;

    volatile int _taskState;
    volatile int _activateTimes;
    volatile LLBC_Handle _threadGroupHandle;
    volatile LLBC_ThreadId _activateThreadId;
    LLBC_ThreadMgr *_threadMgr;

    volatile int _threadNum;
    volatile int _activatingThreadNum;
    volatile int _inSvcMethThreadNum;
    volatile int _activatedThreadNum;

    static thread_local int _processorId;
};

/**
 * \brief Task class encapsulation.
 */
class LLBC_EXPORT LLBC_Task : public LLBC_TaskBase<LLBC_TaskQueue>{};

/**
 * \brief Task class encapsulation.
 */
class LLBC_EXPORT LLBC_MultiThreadTask : public LLBC_TaskBase<LLBC_MultiThreadTaskQueue>{};

__LLBC_NS_END

#include "llbc/core/thread/TaskInl.h"


