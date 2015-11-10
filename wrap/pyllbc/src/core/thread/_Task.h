/**
 * @file    _Task.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Common.h"

#include "pyllbc/core/thread/PyTask.h"
#include "pyllbc/core/thread/PyTaskMgr.h"

LLBC_EXTERN_C PyObject *_pyllbc_createtask(PyObject *self, PyObject *args)
{
    char *script;
    if (!PyArg_ParseTuple(args, "s", &script))
        return NULL;

    int taskId = pyllbc_s_TaskMgr->CreateTask(script);
    if (taskId < 0)
        return NULL;

    return Py_BuildValue("i", taskId);
}

LLBC_EXTERN_C PyObject *_pyllbc_currenttaskid(PyObject *self, PyObject *args)
{
    int taskId = pyllbc_s_TaskMgr->GetCurrentTaskId();
    if (taskId < 0)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return NULL;
    }

    return Py_BuildValue("i", taskId);
}

LLBC_EXTERN_C PyObject *_pyllbc_gettaskcount(PyObject *self, PyObject *args)
{
    return Py_BuildValue("I", pyllbc_s_TaskMgr->GetTaskCount());
}

LLBC_EXTERN_C PyObject *_pyllbc_istaskexist(PyObject *self, PyObject *args)
{
    int taskId;
    if (!PyArg_ParseTuple(args, "i", &taskId))
        return NULL;

    const bool exist = 
        pyllbc_s_TaskMgr->IsTaskExist(taskId);

    return PyBool_FromLong(exist ? 1 : 0);
}

LLBC_EXTERN_C PyObject *_pyllbc_istaskactivated(PyObject *self, PyObject *args)
{
    int taskId;
    if (!PyArg_ParseTuple(args, "i", &taskId))
        return NULL;

    const bool activated = 
        pyllbc_s_TaskMgr->IsTaskActivated(taskId);

    return PyBool_FromLong(activated ? 1 : 0);
}

LLBC_EXTERN_C PyObject *_pyllbc_pushmsg(PyObject *self, PyObject *args)
{
    /* Fill stuff here */
    return NULL;
}

LLBC_EXTERN_C PyObject *_pyllbc_popmsg(PyObject *self, PyObject *args)
{
    /* Fill stuff here */
    return NULL;
}
