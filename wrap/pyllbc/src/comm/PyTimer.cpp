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

#include "pyllbc/common/Export.h"

#include "pyllbc/comm/PyTimer.h"

namespace
{
    typedef LLBC_Timer Base;

    static int __NormalizeObj(PyObject *obj, PyObject *&method, LLBC_String &methodDesc, PyObject *&boundedObj, LLBC_String &boundedObjDesc)
    {
        static const char *couldNotCreateRefStr = "could not create timer handler weakref object";

        if (UNLIKELY(!PyCallable_Check(obj)))
        {
            const LLBC_String objDesc = pyllbc_ObjUtil::GetObjStr(obj);
            if (LIKELY(!PyErr_Occurred()))
            {
                LLBC_String errStr;
                pyllbc_SetError(errStr.format("timeout/cancel handler must callable, handler: %s", objDesc.c_str()));
            }

            method = boundedObj = nullptr;
            return LLBC_FAILED;
        }
        else if (PyMethod_Check(obj))
        {
            if (!(boundedObj = PyMethod_Self(obj)))  // Borrowed reference.
            {
                const LLBC_String objDesc = pyllbc_ObjUtil::GetObjStr(obj);
                if (LIKELY(!PyErr_Occurred()))
                {
                    LLBC_String errStr;
                    pyllbc_SetError(errStr.format(
                        "could not pass unbound method object to timer, obj: %s", objDesc.c_str()));
                }

                method = nullptr;
                return LLBC_FAILED;
            }

            method = PyMethod_Function(obj); // Borrowed reference.
        }
        else
        {
            method = obj;
            boundedObj = nullptr;
        }

        methodDesc = pyllbc_ObjUtil::GetObjStr(method);
        if (boundedObj)
            boundedObjDesc = pyllbc_ObjUtil::GetObjStr(boundedObj);

        PyObject *refMethod = PyWeakref_NewRef(method, nullptr);
        if (UNLIKELY(!refMethod))
        {
            pyllbc_TransferPyError(couldNotCreateRefStr);
            method = boundedObj = nullptr;

            return LLBC_FAILED;
        }

        method = refMethod;
        if (boundedObj)
        {
            PyObject *refBoundedObj = PyWeakref_NewRef(boundedObj, nullptr);
            if (UNLIKELY(!refBoundedObj))
            {
                pyllbc_TransferPyError(couldNotCreateRefStr);

                Py_DECREF(method);
                method = boundedObj = nullptr;

                return LLBC_FAILED;
            }

            boundedObj = refBoundedObj;
        }

        return LLBC_OK;
    }

    static int __CallObj(PyObject *boundedObj,
                         const LLBC_String &boundedObjDesc,
                         PyObject *method,
                         const LLBC_String &methodDesc,
                         PyObject *timer,
                         const LLBC_TimerId &timerId,
                         bool isTimeout,
                         bool ignoredDeadRef,
                         bool &needReschedule)
    {
        PyObject *rawMethod = PyWeakref_GET_OBJECT(method); // Borrowed reference.
        if (rawMethod == Py_None)
        {
            if (LIKELY(!PyErr_Occurred()))
            {
                LLBC_String errStr;
                errStr.format("timer %s handler weakref was dead, "
                    "handler: %s, bounded obj: %s, timer id: %llu, please check code logic", 
                    isTimeout?"timeout":"cancel", methodDesc.c_str(), boundedObjDesc.c_str(), timerId);
                if (isTimeout)
                    errStr.append(", timer will automatic stop");

                pyllbc_SetError(errStr);
            }

                return LLBC_FAILED;
        }

        PyObject *rawBoundedObj = nullptr;
        if (boundedObj)
        {
            if ((rawBoundedObj = PyWeakref_GET_OBJECT(boundedObj)) == Py_None) // Borrowed reference.
            {
                if (LIKELY(!PyErr_Occurred()))
                {
                    //! ignore dead weak-ref error
                    if (ignoredDeadRef)
                    {
                        needReschedule = false;
                        return LLBC_OK;
                    }
                    else
                    {
                        LLBC_String errStr;
                        errStr.format("timer %s handler bounded object dead, "
                            "handler: %s, bounded obj: %s, timer id: %llu, please check code logic",
                            isTimeout?"timeout":"cancel", methodDesc.c_str(), boundedObjDesc.c_str(), timerId);
                        if (isTimeout)
                            errStr.append(", timer will automatic stop");

                        pyllbc_SetError(errStr);
                    }
                }

                return LLBC_FAILED;
            }
        }

        PyObject *pyRtn;
        Py_INCREF(timer);
        if (rawBoundedObj)
            pyRtn = PyObject_CallFunctionObjArgs(rawMethod, rawBoundedObj, timer, nullptr);
        else
            pyRtn = PyObject_CallFunctionObjArgs(rawMethod, timer, nullptr);
        Py_DECREF(timer);

        if (!pyRtn)
        {
            pyllbc_TransferPyError();
            return LLBC_FAILED;
        }

        needReschedule = true;
        Py_DECREF(pyRtn);

        return LLBC_OK;
    }
}

pyllbc_Timer::pyllbc_Timer(PyObject *pyTimer, PyObject *timeoutCallable, PyObject *cancelCallable)
: Base()
, _pyTimer(pyTimer)
, _ignoredDeadRef(false)
{
    ASSERT(PyCallable_Check(timeoutCallable) && "timer's timeout obj must callable");
    if (__NormalizeObj(timeoutCallable,
                       _timeoutCallable,
                       _timeoutCallableDesc,
                       _boundedTimeOutObj,
                       _boundedTimeOutObjDesc) != LLBC_OK)
        return;

    if (cancelCallable)
    {
        if (__NormalizeObj(cancelCallable,
                           _cancelCallable,
                           _cancelCallableDesc,
                           _boundedCancelObj,
                           _boundedCancelObjDesc) != LLBC_OK)
            return;
    }
    else
    {
        _cancelCallable = nullptr;
        _boundedCancelObj = nullptr;
    }

}

pyllbc_Timer::~pyllbc_Timer()
{
    Cancel();

    Py_XDECREF(_timeoutCallable);
    Py_XDECREF(_boundedTimeOutObj);

    Py_XDECREF(_cancelCallable);
    Py_XDECREF(_boundedCancelObj);
}

bool pyllbc_Timer::IsIgnoredDeadRef() const
{
    return _ignoredDeadRef;
}

void pyllbc_Timer::SetIgnoredDeadRef(bool flag)
{
    _ignoredDeadRef = flag;
}

int pyllbc_Timer::Schedule(sint64 dueTime, sint64 period)
{
    if (Base::Schedule(LLBC_TimeSpan::FromMillis(dueTime), LLBC_TimeSpan::FromMillis(period)) != LLBC_OK)
    {
        const LLBC_String desc = ToString();
        pyllbc_TransferLLBCError(__FILE__, __LINE__, desc);
        return LLBC_FAILED;
    }

    Py_INCREF(_pyTimer);

    return LLBC_OK;
}

LLBC_String pyllbc_Timer::ToString() const
{
    LLBC_String baseDesc = Base::ToString();
    LLBC_String &desc = baseDesc.append_format(
        ", timeout callable: %s, timeout callable bound obj: %s", 
        _timeoutCallableDesc.c_str(), _boundedTimeOutObjDesc.c_str());
    if (_cancelCallable)
        desc.append_format(", cancel callable: %s, cancel callable bound obj: %s",
                           _cancelCallableDesc.c_str(), _boundedCancelObjDesc.c_str());

    return desc;
}

void pyllbc_Timer::OnTimeout()
{
    bool reschedule = false;
    if (__CallObj(_boundedTimeOutObj,
                  _boundedTimeOutObjDesc,
                  _timeoutCallable,
                  _timeoutCallableDesc,
                  _pyTimer,
                  GetTimerId(),
                  true,
                  _ignoredDeadRef,
                  reschedule) != LLBC_OK)
    {
        Py_DECREF(_pyTimer);
        Cancel();
        return;
    }

    if (!reschedule)
    {
        Cancel();
        Py_DECREF(_pyTimer);
    }
}

void pyllbc_Timer::OnCancel()
{
    if (!_cancelCallable)
    {
        Py_DECREF(_pyTimer);
        return;
    }

    bool reschedule;
    __CallObj(_boundedCancelObj,
              _boundedCancelObjDesc,
              _cancelCallable,
              _cancelCallableDesc,
              _pyTimer,
              GetTimerId(),
              false,
              _ignoredDeadRef,
              reschedule);

    Py_DECREF(_pyTimer);
}
