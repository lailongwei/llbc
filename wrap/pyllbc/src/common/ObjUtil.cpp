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

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/ObjUtil.h"
#include "pyllbc.h"

namespace
{
    LLBC_String __g_invalidObjStr;
}

LLBC_String pyllbc_ObjUtil::GetObjStr(PyObject *obj)
{
    PyObject *pyStr = PyObject_Str(obj);
    if (UNLIKELY(!pyStr))
    {
        pyllbc_TransferPyError();
        return __g_invalidObjStr;
    }

    char *strBeg;
    Py_ssize_t strLen;
    if (UNLIKELY(PyString_AsStringAndSize(pyStr, &strBeg, &strLen) == -1))
    {
        pyllbc_TransferPyError("When get python object string representation");
        Py_DECREF(pyStr);

        return __g_invalidObjStr;
    }

    LLBC_String str(strBeg, strLen);
    Py_DECREF(pyStr); //! Must later DECREF.

    return str;
}

int pyllbc_ObjUtil::Obj2Variant(PyObject *obj, LLBC_Variant &var)
{
    int ty = pyllbc_TypeDetector::Detect(obj);
    switch (ty)
    {
    case PYLLBC_NONE_OBJ:
        var.BecomeNil();
        return LLBC_OK;

    case PYLLBC_BOOL_OBJ:
        var = PyInt_AsLong(obj) ? true : false;
        return LLBC_OK;

    case PYLLBC_INT_OBJ:
        var = PyInt_AsLong(obj);
        return LLBC_OK;

    case PYLLBC_LONG_OBJ:
        var = static_cast<sint64>(PyLong_AsLongLong(obj));
        return LLBC_OK;

    case PYLLBC_FLOAT_OBJ:
        var = PyFloat_AsDouble(obj);
        return LLBC_OK;

    case PYLLBC_STR_OBJ:
    {
        char *str;
        Py_ssize_t strLen;
        PyString_AsStringAndSize(obj, &str, &strLen);

        var.BecomeStr();
        LLBC_String *&holdedStr = var.GetMutableHolder()->data.obj.str;
        if (!holdedStr)
            holdedStr = new LLBC_String(str, static_cast<size_t>(strLen));
        else
            holdedStr->assign(str, static_cast<size_t>(strLen));

        return LLBC_OK;
    }

    case PYLLBC_UNICODE_OBJ:
    {
        PyObject *utf8Obj = PyUnicode_AsUTF8String(obj); // New reference.
        if (UNLIKELY(!utf8Obj))
        {
            pyllbc_TransferPyError("When convert unicode object to utf8 string object");
            return LLBC_FAILED;
        }

        char *str;
        Py_ssize_t strLen;
        PyString_AsStringAndSize(utf8Obj, &str, &strLen);

        var.BecomeStr();
        var.GetMutableHolder()->data.obj.str->assign(str, static_cast<size_t>(strLen));

        Py_DECREF(utf8Obj);

        return LLBC_OK;
    }

    case PYLLBC_LIST_OBJ:
    case PYLLBC_TUPLE_OBJ:
    {
        var.BecomeSeq();

        PyObject *seq = PySequence_Fast(obj, nullptr); // New reference
        Py_ssize_t seqSize = PySequence_Fast_GET_SIZE(obj);
        for (Py_ssize_t i = 0; i != seqSize; ++i)
        {
            PyObject *elem = PySequence_Fast_GET_ITEM(seq, i); // Borrowed reference

            var.SeqPushBack(LLBC_Variant::nil);
            LLBC_Variant &elemVar = var.SeqBack();
            if (Obj2Variant(elem, elemVar) != LLBC_OK)
            {
                var.SeqPopBack();
                Py_DECREF(seq);

                return LLBC_FAILED;
            }
        }

        Py_DECREF(seq);

        return LLBC_OK;
    }

    case PYLLBC_DICT_OBJ:
    {
        var.BecomeDict();

        Py_ssize_t pos = 0;
        PyObject *key, *value;
        while (PyDict_Next(obj, &pos, &key, &value))
        {
            LLBC_Variant nativeKey;
            if (UNLIKELY(Obj2Variant(key, nativeKey) != LLBC_OK))
                return LLBC_FAILED;

            LLBC_Variant nativeValue;
            if (UNLIKELY(Obj2Variant(value, nativeValue) != LLBC_OK))
                return LLBC_FAILED;

            var.DictInsert(nativeKey, nativeValue);
        }

        return LLBC_OK;
    }

    default:
    {
        pyllbc_SetError("Not supported python object type when convert to LLBC_Variant", LLBC_ERROR_NOT_SUPPORT);
        return LLBC_FAILED;
    }
    }
}

PyObject *pyllbc_ObjUtil::Variant2Obj(const LLBC_Variant &var)
{
    if (var.IsNil()) // Nil type
    {
        Py_RETURN_NONE;
    }
    else if (var.IsRaw()) // Raw type(bool, integer, float)
    {
        if (var.IsBool())
        {
            if (var.AsBool())
                Py_RETURN_TRUE;
            else
                Py_RETURN_FALSE;
        }
        else if (var.IsInt8() ||
                 var.IsUInt8() ||
                 var.IsInt16() ||
                 var.IsUInt16() ||
                 var.IsInt32() ||
                 var.IsLong())
        {
            return PyInt_FromLong(var.AsLong());
        }
        else if (var.IsULong())
        {
            return PyLong_FromUnsignedLong(var.AsULong());
        }
        else if (var.IsUInt32() ||
                 var.IsInt64())
        {
            return PyLong_FromLongLong(var.AsInt64());
        }
        else if (var.IsPtr() ||
                 var.IsUInt64())
        {
            return PyLong_FromUnsignedLongLong(var.AsUInt64());
        }
        else if (var.IsFloat() ||
                 var.IsDouble())
        {
            return PyFloat_FromDouble(var.AsDouble());
        }
    }
    else if (var.IsStr()) // Str type
    {
        if (!var.IsEmpty())
            return PyString_FromStringAndSize(var.GetHolder().data.obj.str->c_str(), var.GetHolder().data.obj.str->size());
        else
            return PyString_FromStringAndSize(nullptr, 0);
    }
    else if (var.IsSeq()) // Seq type
    {
        const size_t seqSize = var.Size();
        PyObject *pyList = PyList_New(seqSize);
        for (size_t i = 0; i != seqSize; ++i)
        {
            const LLBC_Variant &elem = var[i];
            PyObject *pyElem = Variant2Obj(elem);
            if (!pyElem)
            {
                Py_DECREF(pyList);
                return nullptr;
            }

            PyList_SetItem(pyList, i, pyElem); // Steals pyElem reference.
        }

        return pyList;
    }
    else if (var.IsDict()) // Dict type
    {
        PyObject *pyDict = PyDict_New();
        for (LLBC_Variant::DictConstIter it = var.DictBegin();
             it != var.DictEnd();
             ++it)
        {
            const LLBC_Variant &key = it->first;
            const LLBC_Variant &value = it->second;
            PyObject *pyKey = Variant2Obj(key); // New reference.
            if (!pyKey)
            {
                Py_DECREF(pyDict);
                return nullptr;
            }

            PyObject *pyValue = Variant2Obj(value); // New reference.
            if (!pyValue)
            {
                Py_DECREF(pyKey);
                Py_DECREF(pyDict);

                return nullptr;
            }

            if (PyDict_SetItem(pyDict, pyKey, pyValue) != 0) // No steal reference.
            {
                Py_DECREF(pyValue);
                Py_DECREF(pyKey);
                Py_DECREF(pyDict);
                pyllbc_TransferPyError("When set item to dict");

                return nullptr;
            }

            Py_DECREF(pyValue);
            Py_DECREF(pyKey);
        }

        return pyDict;
    }

    pyllbc_SetError("Not support LLBC_Variant type when convert to python object", LLBC_ERROR_NOT_SUPPORT);
    return nullptr;
}
