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


#include "pyllbc/comm/PyTimer.h"

LLBC_EXTERN_C PyObject *_pyllbc_NewPyTimer(PyObject *self, PyObject *args)
{
    PyObject *pyObj;
    PyObject *timeoutCallable, *cancelCallable = nullptr;
    if (!PyArg_ParseTuple(args, "OO|O", &pyObj, &timeoutCallable, &cancelCallable))
        return nullptr;

    pyllbc_Timer *timer = new pyllbc_Timer(pyObj, timeoutCallable, cancelCallable);

    return PyLong_FromVoidPtr(timer);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelPyTimer(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    delete timer;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerIsIgnoredDeadRef(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    PyObject *retVal = 
        timer->IsIgnoredDeadRef() ? Py_True : Py_False;
    Py_INCREF(retVal);

    return retVal;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerSetIgnoredDeadRef(PyObject *self, PyObject *args)
{
    PyObject *flagObj;
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "O", &flagObj);

    const bool flag = !!PyObject_IsTrue(flagObj);
    timer->SetIgnoredDeadRef(flag);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetFirstPeriod(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    return PyLong_FromLongLong(timer->GetFirstPeriod().GetTotalMillis());
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetPeriod(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    return PyLong_FromLongLong(timer->GetPeriod().GetTotalMillis());
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetTimerId(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    return PyLong_FromUnsignedLongLong(timer->GetTimerId());
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerSchedule(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PY_LONG_LONG dueTime, period;
    long triggerCount = -1;
    PYLLBC_ParseCObjBeginArgs(timer, "LL|l", &dueTime, &period, &triggerCount);

    if (triggerCount == 0)
        triggerCount = -1;

    if (timer->Schedule(LLBC_TimeSpan::FromMillis(dueTime), 
                        LLBC_TimeSpan::FromMillis(period),
                        static_cast<size_t>(triggerCount)) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, timer->ToString());
        return nullptr;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerCancel(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    if (timer->Cancel() != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, timer->ToString());
        return nullptr;
    }

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerIsScheduled(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    PyObject *rtn = timer->IsScheduled() ? Py_True : Py_False;
    Py_INCREF(rtn);

    return rtn;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerIsHandlingTimeout(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    PyObject *rtn = timer->IsHandlingTimeout() ? Py_True : Py_False;
    Py_INCREF(rtn);

    return rtn;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerIsHandlingCancel(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    PyObject *rtn = timer->IsHandlingCancel() ? Py_True : Py_False;
    Py_INCREF(rtn);

    return rtn;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetTotalTriggerCount(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    return PyLong_FromLong(static_cast<long>(timer->GetTotalTriggerCount()));
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerGetTriggeredCount(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    return PyLong_FromLong(static_cast<long>(timer->GetTriggeredCount()));
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerToString(PyObject *self, PyObject *args)
{
    pyllbc_Timer *timer;
    PYLLBC_ParseCObjBeginArgs(timer, "");

    const LLBC_String timerStrRepr = timer->ToString();
    return PyString_FromStringAndSize(timerStrRepr.c_str(), timerStrRepr.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_PyTimerUpdateAllTimers(PyObject *self, PyObject *args)
{
    pyllbc_s_TimerScheduler->Schedule();
    if (PyErr_Occurred())
        return nullptr;

    Py_RETURN_NONE;
}
