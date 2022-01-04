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


#include "pyllbc/comm/PyObjCoder.h"

LLBC_EXTERN_C PyObject *_pyllbc_o2j(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return nullptr;


    std::string j;
    if (pyllbc_ObjCoder::Encode(obj, j) != LLBC_OK)
        return nullptr;

    return PyString_FromStringAndSize(j.c_str(), j.length());
}

LLBC_EXTERN_C PyObject *_pyllbc_j2o(PyObject *self, PyObject *args)
{
    char *jStr;
    int strLen;
    if (!PyArg_ParseTuple(args, "s#", &jStr, &strLen))
        return nullptr;

    if (strLen == 0)
    {
        pyllbc_SetError("no data to be decode");
        return nullptr;
    }

    PyObject *o;
    if (pyllbc_ObjCoder::Decode(std::string(jStr, strLen), o) != LLBC_OK)
        return nullptr;

    return o;
}
