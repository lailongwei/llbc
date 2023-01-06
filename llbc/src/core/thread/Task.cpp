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
#include "llbc/core/thread/ThreadManager.h"
#include "llbc/core/thread/Guard.h"
#include "llbc/core/thread/Task.h"

__LLBC_INTERNAL_NS_BEGIN

int static __LLBC_TaskEntry(void *arg)
{
    LLBC_NS LLBC_Task *task = reinterpret_cast<LLBC_NS LLBC_Task *>(arg);

    LLBC_NS __LLBC_LibTls *tls = LLBC_NS __LLBC_GetLibTls();

    tls->coreTls.task = task;

    task->OnTaskThreadStart();
    task->Svc();
    task->OnTaskThreadStop();

    tls->coreTls.task = nullptr;

    return 0;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_Task::LLBC_Task(LLBC_ThreadManager *threadMgr)
: _threadNum(0)
, _curThreadNum(0)
, _startCompleted(false)
, _threadManager(threadMgr ? threadMgr : LLBC_ThreadManagerSingleton)
, _taskThreads(nullptr)
{
}

LLBC_Task::~LLBC_Task()
{
    Wait();
    LLBC_XFree(_taskThreads);
}

int LLBC_Task::Activate(int threadNum,
                            int flags,
                            int priority,
                            LLBC_Handle groupHandle,
                            const int stackSize[])
{
    _lock.Lock();

    LLBC_XFree(_taskThreads);
    _taskThreads = LLBC_Calloc(LLBC_Handle, threadNum * sizeof(LLBC_Handle));
    if (_threadManager->CreateThreads(threadNum,
                                      &LLBC_INTERNAL_NS __LLBC_TaskEntry,
                                      this,
                                      flags,
                                      priority,
                                      stackSize,
                                      this,
                                      groupHandle,
                                      nullptr,
                                      _taskThreads) == LLBC_INVALID_HANDLE)
    {
        LLBC_XFree(_taskThreads);
        _lock.Unlock();

        return LLBC_FAILED;
    }

    _threadNum = threadNum;

    _lock.Unlock();

    while (!_startCompleted)
        LLBC_ThreadManager::Sleep(1);

    return LLBC_OK;
}

bool LLBC_Task::IsActivated() const
{
    LLBC_LockGuard guard(const_cast<LLBC_Task *>(this)->_lock);
    return _threadNum != 0;
}

int LLBC_Task::GetThreadCount() const
{
    LLBC_Task *ncThis = const_cast<LLBC_Task *>(this);
    LLBC_LockGuard guard(ncThis->_lock);

    return _threadNum;
}

int LLBC_Task::Wait()
{
    if (!IsActivated())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    return _threadManager->WaitTask(this);
}

int LLBC_Task::Suspend()
{
    if (!IsActivated())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    return _threadManager->SuspendTask(this);
}

int LLBC_Task::Resume()
{
    if (!IsActivated())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    return _threadManager->ResumeTask(this);
}

int LLBC_Task::Cancel()
{
    if (!IsActivated())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    return _threadManager->CancelTask(this);
}

int LLBC_Task::Kill(int signo)
{
    if (!IsActivated())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_INIT);
        return LLBC_FAILED;
    }

    return _threadManager->KillTask(this, signo);
}

void LLBC_Task::OnTaskThreadStart()
{
    LLBC_LockGuard guard(_lock);
    if (++_curThreadNum == _threadNum)
        _startCompleted = true;
}

void LLBC_Task::OnTaskThreadStop()
{
    while (!_startCompleted)
        LLBC_ThreadManager::Sleep(1);

    _lock.Lock();
    if (--_curThreadNum == 0)
    {
        Cleanup();
        InternalCleanup();

        _lock.Unlock();

        return;
    }

    _lock.Unlock();
}

void LLBC_Task::InternalCleanup()
{
    if (_threadNum == 0)
        return;

    _threadNum = 0;
    _curThreadNum = 0;
    _startCompleted = false;

    LLBC_XFree(_taskThreads);

    _msgQueue.Cleanup();
}

void LLBC_Task::GetTaskThreads(std::vector<LLBC_Handle> &taskThreads)
{
    if (!IsActivated())
        return;

    while (!_startCompleted)
        LLBC_ThreadManager::Sleep(1);

    LLBC_LockGuard guard(_lock);
    for (int i = 0; i != _threadNum; ++i)
        taskThreads.push_back(_taskThreads[i]);
}

__LLBC_NS_END
