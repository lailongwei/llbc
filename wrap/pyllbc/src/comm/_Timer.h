/**
 * @file    _Timer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/09/05
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/comm/PyTimer.h"

LLBC_EXTERN_C PyObject *_pyllbc_NewPyTimer(PyObject *self, PyObject *args)
{
    PyObject *pyObj;
    PyObject *timeoutCallable, *cancelCallable = NULL;
    if (!PyArg_ParseTuple(args, "OO|O", &pyObj, &timeoutCallable, &cancelCallable))
        return NULL;

    pyllbc_Timer *timer = new pyllbc_Timer(pyObj, timeoutCallable, cancelCallable);

    return Py_BuildValue("l", timer);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelPyTimer(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "l", &timer))
        return NULL;

    delete timer;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerIsIgnoredDeadRef(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "l", &timer))
        return NULL;

    PyObject *retVal = 
        timer->IsIgnoredDeadRef() ? Py_True : Py_False;
    Py_INCREF(retVal);

    return retVal;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerSetIgnoredDeadRef(PyObject *self, PyObject *args)
{
    PyObject *flagObj;
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "lO", &timer, &flagObj))
        return NULL;

    const bool flag = !!PyObject_IsTrue(flagObj);
    timer->SetIgnoredDeadRef(flag);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetDueTime(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "l", &timer))
        return NULL;

    return Py_BuildValue("K", timer->GetDueTime());
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetPeriod(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "l", &timer))
        return NULL;

    return Py_BuildValue("K", timer->GetPeriod());
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetTimerId(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "l", &timer))
        return NULL;

    return Py_BuildValue("K", timer->GetTimerId());
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerSchedule(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    uint64 dueTime, period;
    if (!PyArg_ParseTuple(args, "lKK", &timer, &dueTime, &period))
        return NULL;

    if (timer->Schedule(dueTime, period) != LLBC_RTN_OK)
    {
        const LLBC_String timerDesc = timer->ToString();
        pyllbc_TransferLLBCError(__FILE__, __LINE__, timerDesc);

        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerCancel(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "l", &timer))
        return NULL;

    if (timer->Cancel() != LLBC_RTN_OK)
    {
        const LLBC_String timerDesc = timer->ToString();
        pyllbc_TransferLLBCError(__FILE__, __LINE__, timerDesc);

        return NULL;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerIsScheduling(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    if (!PyArg_ParseTuple(args, "l", &timer))
        return NULL;

    PyObject *rtn = timer->IsScheduling() ? Py_True : Py_False;
    Py_INCREF(rtn);

    return rtn;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerUpdateAllTimers(PyObject *self, PyObject *args)
{
    pyllbc_s_TimerScheduler->Schedule();
    if (PyErr_Occurred())
        return NULL;

    Py_RETURN_NONE;
}
