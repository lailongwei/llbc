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

#include "pyllbc/common/LibHeader.h"

LLBC_EXTERN_C PyObject *_pyllbc_output(PyObject *self, PyObject *args)
{
    PyObject *obj = nullptr;
    int outputDest = 1;
    if (!PyArg_ParseTuple(args, "|Oi", &obj, &outputDest))
        return nullptr;

    if (!obj)
        Py_RETURN_NONE;

    PyObject *strObj = obj;
    if (!PyObject_IsInstance(obj, PYLLBC_STR_CLS))
    {
        if (!(strObj = PyObject_Str(obj)))
            return nullptr;
    }
    else
    {
        Py_INCREF(strObj);
    }

    char *str;
    if (!PyArg_Parse(strObj, "s", &str))
    {
        Py_DECREF(strObj);
        return nullptr;
    }

    LLBC_Print("%s", str);
    Py_DECREF(strObj);

    Py_RETURN_NONE;
}
