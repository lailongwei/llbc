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

#pragma once

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/PyModule.h"

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, PyObject *&val)
{
    if (!(val = PyObject_GetAttrString(_obj, name.c_str())))
    {
        pyllbc_SetError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, double &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    if (!PyArg_ParseTuple(oVal, "d", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, float &val)
{
    double dbVal;
    if (this->GetAttr(name, dbVal) != LLBC_OK)
        return LLBC_FAILED;

    val = static_cast<float>(dbVal);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, LLBC_String &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    char *str;
    if (!PyArg_Parse(oVal, "s", &str))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    val = str;
    Py_DECREF(oVal);

    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, sint64 &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    if (!PyArg_Parse(oVal, "L", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Py_DECREF(oVal);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, uint64 &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    if (!PyArg_Parse(oVal, "K", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Py_DECREF(oVal);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, long &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    if (!PyArg_Parse(oVal, "l", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Py_DECREF(oVal);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, LLBC_NS ulong &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    if (!PyArg_Parse(oVal, "k", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Py_DECREF(oVal);
    return LLBC_OK;
}
template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, sint32 &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;
    
    if (!PyArg_Parse(oVal, "i", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Py_DECREF(oVal);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, uint32 &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    if (!PyArg_Parse(oVal, "I", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Py_DECREF(oVal);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, sint16 &val)
{
    sint32 i32Val;
    if (this->GetAttr(name, i32Val) != LLBC_OK)
        return LLBC_FAILED;

    val = static_cast<sint16>(i32Val);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, uint16 &val)
{
    uint32 u32Val;
    if (this->GetAttr(name, u32Val) != LLBC_OK)
        return LLBC_FAILED;

    val = static_cast<uint16>(u32Val);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, sint8 &val)
{
    PyObject *oVal;
    if (this->GetAttr(name, oVal) != LLBC_OK)
        return LLBC_FAILED;

    if (!PyArg_Parse(oVal, "b", &val))
    {
        Py_DECREF(oVal);

        pyllbc_SetError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    Py_DECREF(oVal);
    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, uint8 &val)
{
    uint32 u32Val;
    if (this->GetAttr(name, u32Val) != LLBC_OK)
        return LLBC_FAILED;

    val = static_cast<uint8>(u32Val);
    return LLBC_OK;
}

template <typename _Val>
inline int pyllbc_ObjAttrOptr::GetAttr(const LLBC_String &name, _Val &val)
{
    static const LLBC_String err = 
        "pyllbc_ObjAttrOptr::GetAttr<_Val>(): Unspecifilization template call!";

    ASSERT(false && err.c_str());
    
    pyllbc_SetError(err.c_str(), LLBC_ERROR_UNKNOWN);

    return LLBC_FAILED;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, PyObject * const &val)
{
    if (PyObject_SetAttrString(_obj, name.c_str(), val) == -1)
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const double &val)
{
    PyObject *oVal = PyFloat_FromDouble(val);
    const int ret = this->SetAttr(name, oVal);

    Py_DECREF(oVal);

    return ret;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const float &val)
{
    return this->SetAttr(name, static_cast<double>(val));
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const LLBC_String &val)
{
    PyObject *oVal = PyString_FromStringAndSize(val.data(), val.size());
    const int ret = this->SetAttr(name, oVal);

    Py_DECREF(oVal);

    return ret;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, char * const &val)
{
    PyObject *oVal = PyString_FromString(val);
    const int ret = this->SetAttr(name, oVal);

    Py_DECREF(oVal);

    return ret;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const sint64 &val)
{
    PyObject *oVal = PyLong_FromLongLong(val);
    const int ret = this->SetAttr(name, oVal);

    Py_DECREF(oVal);

    return ret;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const uint64 &val)
{
    PyObject *oVal = PyLong_FromUnsignedLongLong(val);
    const int ret = this->SetAttr(name, oVal);

    Py_DECREF(oVal);

    return ret;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const long &val)
{
    PyObject *oVal = PyInt_FromLong(val);
    const int ret = this->SetAttr(name, oVal);

    Py_DECREF(oVal);

    return ret;
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const ::llbc::ulong &val)
{
    return this->SetAttr(name, static_cast<uint64>(val));
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const sint32 &val)
{
    return this->SetAttr(name, static_cast<long>(val));
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const uint32 &val)
{
    return this->SetAttr(name, static_cast< ::llbc::ulong>(val));
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const sint16 &val)
{
    return this->SetAttr(name, static_cast<sint32>(val));
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const uint16 &val)
{
    return this->SetAttr(name, static_cast<uint32>(val));
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const sint8 &val)
{
    LLBC_String sVal(1, val);
    return this->SetAttr(name, sVal);
}

template <>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const uint8 &val)
{
    return this->SetAttr(name, static_cast<uint16>(val));
}

template <typename _Val>
inline int pyllbc_ObjAttrOptr::SetAttr(const LLBC_String &name, const _Val &val)
{
    static const LLBC_String err = 
        "pyllbc_ObjAttrOptr::SetAttr<_Val>(): Unspecifilization template call!";

    ASSERT(false && err.c_str());

    pyllbc_SetError(err.c_str(), LLBC_ERROR_UNKNOWN);

    return LLBC_FAILED;
}


