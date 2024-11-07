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

LLBC_EXTERN_C PyObject *_pyllbc_Properties_LoadFromString(PyObject *self, PyObject *args)
{
    char *str;
    int strLen;
    if (!PyArg_ParseTuple(args, "s#", &str, &strLen))
        return nullptr;

    LLBC_String errMsg;
    LLBC_Variant properties;
    if (LLBC_Properties::LoadFromString(LLBC_String(str, strLen), properties, &errMsg) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, errMsg);
        return nullptr;
    }

    return pyllbc_ObjUtil::Variant2Obj(properties);
}


LLBC_EXTERN_C PyObject *_pyllbc_Properties_LoadFromFile(PyObject *self, PyObject *args)
{
    char *file;
    if (!PyArg_ParseTuple(args, "s", &file))
        return nullptr;

    LLBC_String errMsg;
    LLBC_Variant properties;
    if (LLBC_Properties::LoadFromFile(file, properties, &errMsg) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, errMsg);
        return nullptr;
    }

    return pyllbc_ObjUtil::Variant2Obj(properties);
}

static bool __pyllbc_PyPropertiesToProperties(PyObject *pyProperties, LLBC_Variant &properties)
{
    // Check python properties object(Must be Dict).
    if (!pyllbc_TypeDetector::IsDict(pyProperties))
    {
        LLBC_String addiErrInfo;
        addiErrInfo.format("properties must be a dict object, got:%s",
                           pyllbc_ObjUtil::GetObjStr(
                                reinterpret_cast<PyObject *>(Py_TYPE(pyProperties))).c_str());
        pyllbc_SetError(PYLLBC_ERROR_COMMON, __FILE__, __LINE__, addiErrInfo);
        return false;
    }

    // Convert python properties to variant.
    if (pyllbc_ObjUtil::Obj2Variant(pyProperties, properties) != LLBC_OK)
        return false;

    return true;
}

LLBC_EXTERN_C PyObject *_pyllbc_Properties_SaveToString(PyObject *self, PyObject *args)
{
    // Parse args.
    PyObject *pyProperties;
    if (!PyArg_ParseTuple(args, "O", &pyProperties))
        return nullptr;

    // Convert python properties to variant.
    LLBC_Variant properties;
    if (!__pyllbc_PyPropertiesToProperties(pyProperties, properties))
        return nullptr;

    LLBC_String str;
    LLBC_String errMsg;
    if (LLBC_Properties::SaveToString(properties, str, &errMsg) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, errMsg);
        return nullptr;
    }

    return PyString_FromStringAndSize(str.c_str(), str.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_Properties_SaveToFile(PyObject *self, PyObject *args)
{
    // Parse args.
    char *filePath;
    int filePathLen;
    PyObject *pyProperties;
    if (!PyArg_ParseTuple(args, "Os#", &pyProperties, &filePath, &filePathLen))
        return nullptr;

    // Convert python properties to variant.
    LLBC_Variant properties;
    if (!__pyllbc_PyPropertiesToProperties(pyProperties, properties))
        return nullptr;

    // Save properties to file.
    LLBC_String errMsg;
    if (LLBC_Properties::SaveToFile(properties, LLBC_String(filePath, filePathLen), &errMsg) != LLBC_OK)
    {
        pyllbc_TransferLLBCError(__FILE__, __LINE__, errMsg);
        return nullptr;
    }

    Py_RETURN_NONE;
}
