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

LLBC_EXTERN_C PyObject *_pyllbc_NewRandom(PyObject *self, PyObject *args)
{
    int seed;
    if (!PyArg_ParseTuple(args, "i", &seed))
        return nullptr;

    return PyLong_FromLongLong(reinterpret_cast<sint64>(new LLBC_Random(seed)));
}

LLBC_EXTERN_C PyObject *_pyllbc_DelRandom(PyObject *self, PyObject *args)
{
    LLBC_Random *random;
    if (!PyArg_ParseTuple(args, "l", &random))
        return nullptr;

    delete random;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_SeedRand(PyObject *self, PyObject *args)
{
    int seed;
    LLBC_Random *random;
    if (!PyArg_ParseTuple(args, "li", &random, &seed))
        return nullptr;

    random->Seed(seed);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Rand(PyObject *self, PyObject *args)
{
    LLBC_Random *random;
    PyObject *beginObj = nullptr;
    PyObject *endObj = nullptr;
    if (!PyArg_ParseTuple(args, "l|OO", &random, &beginObj, &endObj))
        return nullptr;

    const bool isObj1None = !beginObj || PyObject_IsInstance(beginObj, PYLLBC_NONE_CLS);
    const bool isObj1Int = beginObj && PyObject_IsInstance(beginObj, PYLLBC_INT_CLS);
    const bool isObj1Long = isObj1Int || (beginObj && PyObject_IsInstance(beginObj, PYLLBC_LONG_CLS));
    const bool isObj1Float = beginObj && PyObject_IsInstance(beginObj, PYLLBC_FLOAT_CLS);

    const bool isObj2None = !endObj || PyObject_IsInstance(endObj, PYLLBC_NONE_CLS);
    const bool isObj2Int = endObj && PyObject_IsInstance(endObj, PYLLBC_INT_CLS);
    const bool isObj2Long = isObj2Int || (endObj && PyObject_IsInstance(endObj, PYLLBC_LONG_CLS));
    const bool isObj2Float = endObj && PyObject_IsInstance(endObj, PYLLBC_FLOAT_CLS);

    if ((!isObj1None && !isObj1Int && !isObj1Long && !isObj1Float) ||
        (!isObj2None && !isObj2Int && !isObj2Long && !isObj2Float))
    {
        pyllbc_SetError("rand arg type error", PYLLBC_ERROR_COMMON, PyExc_TypeError);
        return nullptr;
    }

    if (isObj1None)
    {
        if (isObj2None)
            return PyInt_FromLong(random->Rand());
        else if (isObj2Long)
            return PyInt_FromLong(random->Rand(static_cast<int>(PyLong_AsLong(endObj))));
        else
            return PyInt_FromLong(random->Rand(static_cast<int>(PyFloat_AsDouble(endObj))));
    }
    else if (isObj1Long)
    {
        int begin = static_cast<int>(PyLong_AsLong(beginObj));

        if (isObj2None)
            return PyInt_FromLong(random->Rand(begin));
        else if (isObj2Long)
            return PyInt_FromLong(random->Rand(begin, static_cast<int>(PyLong_AsLong(endObj))));
        else
            return PyInt_FromLong(random->Rand(begin, static_cast<int>(PyFloat_AsDouble(endObj))));
    }
    else
    {
        int begin = static_cast<int>(PyFloat_AsDouble(beginObj));

        if (isObj2None)
            return PyInt_FromLong(random->Rand(begin));
        else if (isObj2Long)
            return PyInt_FromLong(random->Rand(begin, static_cast<int>(PyLong_AsLong(endObj))));
        else
            return PyInt_FromLong(random->Rand(begin, static_cast<int>(PyFloat_AsDouble(endObj))));
    }
}

LLBC_EXTERN_C PyObject *_pyllbc_RandReal(PyObject *self, PyObject *args)
{
    LLBC_Random *random;
    if (!PyArg_ParseTuple(args, "l", &random))
        return nullptr;

    return PyFloat_FromDouble(random->RandReal());
}

LLBC_EXTERN_C PyObject *_pyllbc_BoolJudge(PyObject *self, PyObject *args)
{
    LLBC_Random *random;
    if (!PyArg_ParseTuple(args, "l", &random))
        return nullptr;

    PyObject *judgeResult = random->BoolJudge() ? Py_True : Py_False;
    Py_INCREF(judgeResult);

    return judgeResult;
}
