/**
 * @file    Task.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/31
 * @version 1.0
 *
 * @brief
 */
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
     * @param[in] task        - task pointer, default is this.
     * @return int - return 0 if success, otherwise return false.
     */
    virtual int Activate(int threadNum = 1,
                         int flags = LLBC_ThreadFlag::Joinable,
                         int priority = LLBC_ThreadPriority::Normal,
                         LLBC_Handle groupHandle = LLBC_INVALID_HANDLE,
                         const int stack_size[] = NULL,
                         LLBC_BaseTask *task = NULL);

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
    int _threadNum;
    int _curThreadNum;
    volatile bool _startCompleted;
    LLBC_ThreadManager *_threadManager;

    LLBC_SpinLock _lock;

    LLBC_MessageQueue _msgQueue;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_TASK_H__
