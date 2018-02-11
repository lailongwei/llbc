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
