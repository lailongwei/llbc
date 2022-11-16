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

LLBC_EXTERN_C PyObject *_pyllbc_NewBundle(PyObject *self, PyObject *args)
{
    LLBC_Bundle *b = new LLBC_Bundle;
    return Py_BuildValue("l", b);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelBundle(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundlePtr;
    if (!PyArg_ParseTuple(args, "l", &bundlePtr))
        return nullptr;

    delete bundlePtr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetMainBundle(PyObject *self, PyObject *args)
{
    const LLBC_Bundle *b = LLBC_Bundle::GetMainBundle();
    return PyLong_FromLongLong(reinterpret_cast<sint64>(b));
}

LLBC_EXTERN_C PyObject *_pyllbc_GetBundlePath(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundlePtr;
    if (!PyArg_ParseTuple(args, "l", &bundlePtr))
        return nullptr;

    return PyString_FromString(bundlePtr->GetBundlePath().c_str());
}

LLBC_EXTERN_C PyObject *_pyllbc_GetBundleName(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundlePtr;
    if (!PyArg_ParseTuple(args, "l", &bundlePtr))
        return nullptr;

    return PyString_FromString(bundlePtr->GetBundleName().c_str());
}

LLBC_EXTERN_C PyObject *_pyllbc_InitBundle(PyObject *self, PyObject *args)
{
    const char *path;
    LLBC_Bundle *bundle;
    if (!PyArg_ParseTuple(args, "ls", &bundle, &path))
        return nullptr;

    if (bundle->Initialize(path) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return nullptr;
    }

    Py_RETURN_TRUE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetBundleResPath(PyObject *self, PyObject *args)
{
    LLBC_Bundle *bundle;
    const char *name;
    const char *intDir = "";
    const char *extension = "";
    if (!PyArg_ParseTuple(
        args, "ls|ss", &bundle, &name, &extension, &intDir))
        return nullptr;

    LLBC_String resPath = bundle->GetResPath(name, extension, intDir);
    if (resPath.empty())
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__);
        return nullptr;
    }

    return PyString_FromString(resPath.c_str());
}
