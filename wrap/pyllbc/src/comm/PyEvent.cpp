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

#include "pyllbc/comm/PyEvent.h"

PyObject *pyllbc_EventListener::_pyEvCls = nullptr;

pyllbc_EventListener::pyllbc_EventListener()
: _pyListenerObj(nullptr)
, _pyListenerMeth(nullptr)
{
    if (UNLIKELY(!_pyEvCls))
        _pyEvCls = pyllbc_s_TopModule->GetObject("Event");
}

pyllbc_EventListener::~pyllbc_EventListener()
{
    Py_XDECREF(_pyListenerObj);
    Py_XDECREF(_pyListenerMeth);
}

int pyllbc_EventListener::SetPyListener(PyObject *pyListener)
{
    // Reentry check.
    if (UNLIKELY(_pyListenerMeth))
    {
        pyllbc_SetError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Callable check.
    if (UNLIKELY(!PyCallable_Check(pyListener)))
    {
        pyllbc_SetError("event listener must be callable", LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    // Normalize callable.
    if (PyMethod_Check(pyListener))
    {
        PyObject *obj = PyMethod_Self(pyListener); // Borrowed ref.
        PyObject *meth = PyMethod_Function(pyListener); // Borrowed ref.
        if (UNLIKELY(!obj || !meth))
        {
            pyllbc_TransferPyError("When get listener bound self object and function");
            return LLBC_FAILED;
        }

        if (UNLIKELY(!(_pyListenerObj = PyWeakref_NewRef(obj, nullptr))))
        {
            pyllbc_TransferPyError("When create listener bound self object weakref");
            return LLBC_FAILED;
        }
        if (UNLIKELY(!(_pyListenerMeth = PyWeakref_NewRef(meth, nullptr))))
        {
            pyllbc_TransferPyError("When create listener function weakref");
            return LLBC_FAILED;
        }
    }
    else
    {
        if (UNLIKELY(!(_pyListenerMeth = PyWeakref_NewRef(pyListener, nullptr))))
        {
            pyllbc_TransferPyError("When create listener function weakref");
            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

void pyllbc_EventListener::Invoke(LLBC_Event &ev)
{
    if (!_pyListenerMeth)
        return;

    // Method alive check.
    PyObject *meth = PyWeakref_GET_OBJECT(_pyListenerMeth); // Borrow reference.
    if (meth == Py_None)
    {
        Py_DECREF(_pyListenerMeth);
        _pyListenerMeth = nullptr;

        if (_pyListenerObj)
        {
            Py_DECREF(_pyListenerObj);
            _pyListenerObj = nullptr;
        }

        return;
    }
    Py_INCREF(meth);

    // Object alive check.
    PyObject *selfObj = nullptr;
    if (_pyListenerObj)
    {
        selfObj = PyWeakref_GET_OBJECT(_pyListenerObj); // Borrow reference.
        if (selfObj == Py_None)
        {
            Py_DECREF(meth);

            Py_DECREF(_pyListenerObj);
            Py_DECREF(_pyListenerMeth);
            _pyListenerObj = nullptr;
            _pyListenerMeth = nullptr;

            return;
        }

        Py_INCREF(selfObj);
    }

    // Get python layer event object.
    LLBC_Event *cloneEv = nullptr;
    PyObject *pyEv = reinterpret_cast<PyObject *>(ev.GetExtData());
    if (!pyEv) // Not python layer generate event, fire from native layer or other language wrap, clone native event object to create new python layer event object.
    {
        cloneEv = ev.Clone();
        cloneEv->SetDontDelAfterFire(true);
        PyObject *pyEvId = PyInt_FromLong(ev.GetId());
        PyObject *pyCObj = PyLong_FromUnsignedLongLong(reinterpret_cast<uint64>(cloneEv));
        pyEv = PyObject_CallFunctionObjArgs(_pyEvCls, pyEvId, pyCObj, nullptr); // TODO: optimize call performance.
        if (!pyEv)
        {
            Py_DECREF(pyEvId);
            Py_DECREF(pyCObj);

            Py_DECREF(meth);
            Py_XDECREF(selfObj);
            pyllbc_TransferPyError("When create python layer event object");

            return;
        }

        Py_DECREF(pyEvId);
        Py_DECREF(pyCObj);
        cloneEv->SetExtData(pyEv);
    }

    // Call object. TODO: optimize call performance.
    PyObject *ret;
    if (selfObj)
        ret = PyObject_CallFunctionObjArgs(meth, selfObj, pyEv, nullptr);
    else
        ret = PyObject_CallFunctionObjArgs(meth, pyEv, nullptr);
    if (ret == nullptr)
    {
        if (cloneEv)
            Py_DECREF(pyEv);

        Py_DECREF(meth);
        Py_XDECREF(selfObj);

        pyllbc_TransferPyError(LLBC_String().format(
            "When call event listener, evId:%d, listener meth:%s, obj:%s", 
            ev.GetId(), pyllbc_ObjUtil::GetObjStr(meth).c_str(), selfObj ? pyllbc_ObjUtil::GetObjStr(selfObj).c_str() : "None"));
        return;
    }

    if (cloneEv)
        Py_DECREF(pyEv);

    Py_DECREF(meth);
    Py_XDECREF(selfObj);

    Py_DECREF(ret);
}
