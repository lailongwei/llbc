/**
 * @file    PyTaskMgr.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_CORE_THREAD_PY_TASK_MGR_H__
#define __PYLLBC_CORE_THREAD_PY_TASK_MGR_H__

#include "pyllbc/common/Common.h"

class pyllbc_Task;

class LLBC_HIDDEN pyllbc_TaskMgr
{
public:
    pyllbc_TaskMgr();
    ~pyllbc_TaskMgr();

public:
    /**
     * Create python task.
     * @param[in] script - the python script.
     * @return int - return task Id if success, otherwise return -1.
     */
    int CreateTask(const LLBC_String &script);

    /**
     * Get python task.
     * @return pyllbc_Task * - the python task.
     */
    pyllbc_Task *GetTask(int taskId);

    /**
     * Get current python task Id.
     * @return int - the task Id, return -1 if failed.
     */
    int GetCurrentTaskId() const;

    /**
     * Check python task exist or not.
     * @param[in] taskId - the task Id.
     * @return bool - return true if task exist, otherwise return false.
     */
    bool IsTaskExist(int taskId) const;

    /**
     * Check python task activated or not.
     * @param[in] taskId - the task Id.
     * @return bool - return true if task exist, otherwise return false.
     */
    bool IsTaskActivated(int taskId) const;

    /**
     * Get python tasks count.
     * @return uint32 - the task count.
     */
    uint32 GetTaskCount() const;

public:
    /**
     * Push message to task.
     * @param[in] taskId - the task Id.
     * @param[in] msg    - the message block.
     * @reutrn int - return 0 if success, otherwise return -1.
     */
    int PushMsg(int taskId, LLBC_MessageBlock *msg);
    
    /**
     * Pop message from task.
     * @param[in] taskId   - the task Id.
     * @param[out] msg     - the message block.
     * @param[in] interval - wait interval.
     * @return int - return 0 if success, otherwise return -1.
     */
    int PopMsg(int taskId, LLBC_MessageBlock *&msg, int interval = LLBC_INFINITE);

private:
    /**
     * Task created event handler, call by pyllbc_Task class.
     * @param[in] task - triggered event's task.
     */
    void OnTaskCreated(pyllbc_Task *task);

    /**
     * Task destroy event handler, call by pylbc_Task class.
     * @param[in] task - triggered event's task.
     */
    void OnTaskDestroy(pyllbc_Task *task);

    /**
     * Declare the friend class: pyllbc_Task.
     *  Will access methods: OnTaskCreated(), OnTaskDestroy().
     *  Which using to complete task created/destroy bottom process.
     */
    friend class pyllbc_Task;

private:
    typedef std::map<int, pyllbc_Task *> _Tasks;

    LLBC_RecursiveLock _lock;

    _Tasks _tasks;
    int _maxTaskId;

    LLBC_MessageQueue _mainThreadQueue;
};

#define pyllbc_s_TaskMgr LLBC_Singleton<pyllbc_TaskMgr>::Instance()

#endif // !__PYLLBC_CORE_THREAD_PY_TASK_MGR_H__
