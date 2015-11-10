/**
 * @file    PyFrameCallable.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/03/16
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/comm/PyFrameCallable.h"

pyllbc_FrameCallable::pyllbc_FrameCallable(PyObject *svc, PyObject *callable)
: _svc(svc)
, _callable(callable)
{
    Py_INCREF(_callable);
}

pyllbc_FrameCallable::~pyllbc_FrameCallable()
{
    Py_DECREF(_callable);
}

void pyllbc_FrameCallable::Call(LLBC_IService *llbcSvc)
{
    PyObject *ret = PyObject_CallFunctionObjArgs(_callable, _svc, NULL);
    if (ret)
    {
        Py_DECREF(ret);
    }
    else
    {
        const LLBC_String callableDesc = pyllbc_ObjUtil::GetObjStr(_callable);
        pyllbc_TransferPyError(LLBC_String().format("When call callable: %s", callableDesc.c_str()));
    }
}
