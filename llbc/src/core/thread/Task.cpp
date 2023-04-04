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

#include "llbc/core/os/OS_Atomic.h"

#include "llbc/core/thread/MessageBlock.h"
#include "llbc/core/thread/ThreadMgr.h"
#include "llbc/core/thread/Guard.h"
#include "llbc/core/thread/Task.h"

__LLBC_NS_BEGIN

LLBC_Task::LLBC_Task(LLBC_ThreadMgr *threadMgr)
: _taskState(LLBC_TaskState::NotActivated)
, _activateTimes(0)
, _threadGroupHandle(LLBC_INVALID_HANDLE)
, _activateThreadId(LLBC_INVALID_NATIVE_THREAD_ID)
, _threadMgr(threadMgr ? threadMgr : LLBC_ThreadMgrSingleton)

, _threadNum(0)
, _activatingThreadNum(0)
{
}

LLBC_Task::~LLBC_Task()
{
    ASSERT(_taskState == LLBC_TaskState::NotActivated);
}

int LLBC_Task::Activate(int threadNum,
                        int threadPriority,
                        int stackSize)
{
    // Parameters check.
    LLBC_SetErrAndReturnIf(threadNum <= 0, LLBC_ERROR_ARG, LLBC_FAILED);

    // Lock.
    _lock.Lock();

    // Reentry check.
    if (_taskState != LLBC_TaskState::NotActivated)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);

        return LLBC_FAILED;
    }

    // Update task state to <Activating>.
    _taskState = LLBC_TaskState::Activating;

    // Create task threads.
    _threadGroupHandle = _threadMgr->CreateThreads(threadNum,
                                                   LLBC_Delegate<void(void *)>(this, &LLBC_Task::TaskEntry),
                                                   nullptr,
                                                   threadPriority,
                                                   stackSize);
    if (_threadGroupHandle == LLBC_INVALID_HANDLE)
    {
        _taskState = LLBC_TaskState::NotActivated;
        _lock.Unlock();

        return LLBC_FAILED;
    }

    _threadNum = threadNum;

    _lock.Unlock();

    // Wait for all task threads startup.
    while (_activatingThreadNum != _threadNum)
        LLBC_Sleep(0);

    // Record activate thread Id.
    _activateThreadId = LLBC_GetCurrentThreadId();

    // Update task state to activated.
    _taskState = LLBC_TaskState::Activated;
    // Incr activate times.
    ++_activateTimes;

    // Unlock
    _lock.Unlock();

    return LLBC_OK;
}

int LLBC_Task::Wait()
{
    // Task state check.
    _lock.Lock();
    if (_taskState == LLBC_TaskState::NotActivated)
    {
        _lock.Unlock();
        return LLBC_OK;
    }

    // Activate thread check.
    if (LLBC_GetCurrentThreadId() != _activateThreadId)
    {
        _lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);

        return LLBC_FAILED;
    }

    // Get threadGroupHandle.
    const LLBC_Handle groupHandle = _threadGroupHandle;

    // Unlock.
    _lock.Unlock();

    // Wait task threads.
    if (_threadMgr->WaitGroup(groupHandle) == LLBC_OK ||
        LLBC_GetLastError() == LLBC_ERROR_NOT_FOUND)
        return LLBC_OK;

    return LLBC_FAILED;
}

void LLBC_Task::InternalCleanup()
{
    if (_threadNum == 0)
        return;

    _msgQueue.Cleanup();

    _threadNum = 0;
    _activatingThreadNum = 0;

    _threadGroupHandle = LLBC_INVALID_HANDLE;
    _activateThreadId = LLBC_INVALID_NATIVE_THREAD_ID;
    _taskState = LLBC_TaskState::NotActivated;
}

void LLBC_Task::TaskEntry(void *arg)
{
    // Set task object to TLS.
    __LLBC_GetLibTls()->coreTls.task = this;

    // Incr activating thread num.
    (void)LLBC_AtomicFetchAndAdd(&_activatingThreadNum, 1);

    // Waiting for Task::Activate() call finished.
    while (GetTaskState() != LLBC_NS LLBC_TaskState::Activated)
        LLBC_NS LLBC_Sleep(0);

    // Call task Svc() meth.
    // ==========================================
    Svc();
    // ==========================================

    // Set _taskState to Deactivating, if is first stop thread.
    const int preSubThreadNum = LLBC_AtomicFetchAndSub(&_activatingThreadNum, 1);
    if (preSubThreadNum == _threadNum)
    {
        LLBC_AtomicSet(&_taskState, LLBC_TaskState::Deactivating);
    }
    else // Otherwise waiting for _taskState leave Activated status.
    {
        while (_taskState == LLBC_TaskState::Activated)
            LLBC_Sleep(0);
    }

    // If the latest thread is stopped, cleanup task.
    if (preSubThreadNum == 1)
    {
        LLBC_LockGuard guard(_lock);
        Cleanup();
        InternalCleanup();
    }

    // Reset TLS's task member.
    __LLBC_GetLibTls()->coreTls.task = nullptr;
}

__LLBC_NS_END
