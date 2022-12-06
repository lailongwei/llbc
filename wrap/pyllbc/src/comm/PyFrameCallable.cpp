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

void pyllbc_FrameCallable::Call(LLBC_Service *llbcSvc)
{
    PyObject *ret = PyObject_CallFunctionObjArgs(_callable, _svc, nullptr);
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
