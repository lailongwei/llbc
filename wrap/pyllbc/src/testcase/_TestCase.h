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

/**
 * \brief The pyllbc testcase encapsulation.
 */
class LLBC_HIDDEN pyllbc_TestCase : public LLBC_BaseTestCase
{
public:
    pyllbc_TestCase() {  }
    virtual ~pyllbc_TestCase() {  }

public:
    virtual int Run(int argc, char *argv[])
    {
        PyObject *pyObj = reinterpret_cast<PyObject *>(LLBC_Str2Ptr(argv[0]));

        PyObject *callable = nullptr;
        pyllbc_ObjAttrOptr optr(pyObj);
        if (optr.GetAttr("run", callable) != LLBC_OK)
            return LLBC_FAILED;

        if (!PyCallable_Check(callable))
        {
            Py_DECREF(callable);

            pyllbc_SetError("object not callable");
            return LLBC_FAILED;
        }
    
        Py_DECREF(callable);

        PyObject *arg = reinterpret_cast<PyObject *>(LLBC_Str2Ptr(argv[1]));
        PyObject *rtn = PyObject_CallMethod(pyObj, const_cast<char *>("run"), const_cast<char *>("N"), arg);
        if (!rtn)
            return LLBC_FAILED;

        int cRtn = 0;
        if (!PyArg_Parse(rtn, "i", &cRtn))
            cRtn = LLBC_FAILED;

        Py_DECREF(rtn);
        return cRtn;
    }
};

LLBC_EXTERN_C PyObject *_pyllbc_NewTestCase(PyObject *self, PyObject *args)
{
    char *tcName;
    if (!PyArg_ParseTuple(args, "s", &tcName))
        return nullptr;

    pyllbc_TestCase *tc = new pyllbc_TestCase;
    tc->SetName(tcName);

    return Py_BuildValue("l", tc);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelTestCase(PyObject *self, PyObject *args)
{
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "l", &tc))
        return nullptr;

    delete tc;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetTestCaseName(PyObject *self, PyObject *args)
{
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "l", &tc))
        return nullptr;

    return PyString_FromString(tc->GetName().c_str());
}

LLBC_EXTERN_C PyObject *_pyllbc_SetTestCaseName(PyObject *self, PyObject *args)
{
    char *tcName;
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "ls", &tc, &tcName))
        return nullptr;

    tc->SetName(tcName);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_RunTestCase(PyObject *self, PyObject *args)
{
    PyObject *argObj;
    pyllbc_TestCase *tc;
    if (!PyArg_ParseTuple(args, "lN", &tc, &argObj))
        return nullptr;

    char *tcArgs[2] = {nullptr, nullptr};
    tcArgs[0] = const_cast<char *>(LLBC_Num2Str(self).c_str());
    tcArgs[1] = const_cast<char *>(LLBC_Num2Str(argObj).c_str());

    return Py_BuildValue("i", tc->Run(2, tcArgs));
}
