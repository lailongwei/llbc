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

#ifndef __LLBC_CORE_THREAD_TASK_H__
#define __LLBC_CORE_THREAD_TASK_H__

#include "llbc/common/Common.h"

#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/thread/MessageQueue.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_ThreadManager;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Task class encapsulation.
 */
class LLBC_EXPORT LLBC_BaseTask
{
public:
    LLBC_BaseTask(LLBC_ThreadManager *threadMgr = NULL);
    virtual ~LLBC_BaseTask();

public:
    /**
     * Activate current task.
     * @param[in] threadNum   - thread number.
     * @param[in] flags       - thread flag.
     * @param[in] priority    - thread priority.
     * @param[in] groupHandle - thread group handle.
     * @param[in] stack_size  - per thread stack size, in bytes.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Activate(int threadNum = 1,
                         int flags = LLBC_ThreadFlag::Joinable,
                         int priority = LLBC_ThreadPriority::Normal,
                         LLBC_Handle groupHandle = LLBC_INVALID_HANDLE,
                         const int stack_size[] = NULL);

    /**
     * Get thread count.
     * @return int - thread number.
     */
    int GetThreadCount() const;

public:
    /**
     * Wait current task.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Wait();

    /**
     * Suspend current task.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Suspend();

    /**
     * Resume current task.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Resume();

    /**
     * Cancel current task.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Cancel();

    /**
     * Kill current task.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Kill(int signo);

public:
    /**
     * Service method, application should expand it.
     */
    virtual void Svc() = 0;

    /**
     * Cleanup method, when all threads terminated, will call this method to cleanup task.
     */
    virtual void Cleanup() = 0; 

public:
    /**
     * Push message block to task.
     * @param[in] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Push(LLBC_MessageBlock *block);

    /**
     * Pop message block from task.
     * @param[out] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Pop(LLBC_MessageBlock *&block);

    /**
     * Try pop message block from task.
     * @param[out] block - message block.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int TryPop(LLBC_MessageBlock *&block);

    /**
     * Timed pop message block from task.
     * @param[out] block   - message block.
     * @param[in] interval - interval, in milliseconds.
     * @return int -  return 0 if success, otherwise return -1.
     */
    virtual int TimedPop(LLBC_MessageBlock *&block, int interval);

public:
    /**
     * When task thread start, will call this event handler.
     */
    void OnTaskThreadStart();

    /**
     * When task thread stop, will call this event handler.
     */
    void OnTaskThreadStop();

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_BaseTask);

private:
    /**
     * Declare friend class: LLBC_Session.
     *  Access method list:
     *      GetTaskThreads()
     */
    friend class LLBC_ThreadManager;

    /**
     * Get task threads, call by LLBC_TaskManager.
     * @param[out] taskThreads - the task threads.
     */
    void GetTaskThreads(std::vector<LLBC_Handle> &taskThreads);

private:
    int _threadNum;
    int _curThreadNum;
    volatile bool _startCompleted;
    LLBC_ThreadManager *_threadManager;
    LLBC_Handle *_taskThreads;

    LLBC_SpinLock _lock;

    LLBC_MessageQueue _msgQueue;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_TASK_H__
