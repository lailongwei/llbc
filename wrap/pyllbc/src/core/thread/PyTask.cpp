/**
 * @file    PyTask.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/core/thread/PyTask.h"
#include "pyllbc/core/thread/PyTaskMgr.h"

pyllbc_Task::pyllbc_Task(int taskId, const LLBC_String &script)
{
    _taskId = taskId;
    _script.append(script);

    _activated = false;
}

pyllbc_Task::~pyllbc_Task()
{
}

int pyllbc_Task::GetId() const
{
    return _taskId;
}

bool pyllbc_Task::IsActivated() const
{
    return _activated;
}

void pyllbc_Task::Svc()
{
    Py_Initialize();
    PyRun_SimpleString(_script.c_str());

    _activated = true;
    pyllbc_s_TaskMgr->OnTaskCreated(this);
}

void pyllbc_Task::Cleanup()
{
    pyllbc_s_TaskMgr->OnTaskDestroy(this);
    _activated = false;

    Py_Finalize();
}

