/**
 * @file    PyTask.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_CORE_THREAD_PY_TASK_H__
#define __PYLLBC_CORE_THREAD_PY_TASK_H__

#include "pyllbc/common/Common.h"

/**
 * \brief The pyllbc library task class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Task : public LLBC_BaseTask
{
public:
    pyllbc_Task(int taskId, const LLBC_String &script);
    virtual ~pyllbc_Task();

public:
    /**
     * Get the task Id.
     * @return int - the task Id.
     */
    int GetId() const;

    /**
     * Return task activated attribute.
     * @return bool - return true if activated, otherwise return false.
     */
    bool IsActivated() const;

public:
    /**
     * Task service method.
     */
    virtual void Svc();

    /**
     * Task cleanup method.
     */
    virtual void Cleanup();

private:
    int _taskId;
    LLBC_String _script;

    volatile bool _activated;
};

#endif // !__PYLLBC_CORE_THREAD_PY_TASK_H__
