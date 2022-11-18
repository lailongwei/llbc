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
#include "pyllbc/common/PyTypeDetector.h"
#include "pyllbc/common/PackLemma.h"
#include "pyllbc/common/PackLemmaCompiler.h"
#include "pyllbc/common/PyStream.h"

namespace
{
    typedef pyllbc_Stream This;
}

PyObject *pyllbc_Stream::_methEncode = nullptr;
PyObject *pyllbc_Stream::_methDecode = nullptr;

PyObject *pyllbc_Stream::_keyDict = nullptr;
PyObject *pyllbc_Stream::_keySlots = nullptr;

pyllbc_Stream::pyllbc_Stream(PyObject *pyStream, size_t cap)
: _stream(cap)
, _pyStream(pyStream)
{
    if (UNLIKELY(!_methEncode))
    {
        _methEncode = PyString_FromString("encode");
        _methDecode = PyString_FromString("decode");

        _keyDict = PyString_FromString("__dict__");
        _keySlots = PyString_FromString("__slots__");
    }
}

pyllbc_Stream::~pyllbc_Stream()
{
}

PyObject *pyllbc_Stream::GetRaw()
{
    return PyString_FromStringAndSize(reinterpret_cast<
        const char *>(_stream.GetBuf()), _stream.GetPos());
}

int pyllbc_Stream::SetRaw(PyObject *raw)
{
    char *buf = nullptr;
    Py_ssize_t len = 0;

    const pyllbc_ObjType type = pyllbc_TypeDetector::Detect(raw);
    if (type == PYLLBC_STR_OBJ)
    {
        if (PyString_AsStringAndSize(raw, &buf, &len) != 0)
        {
            pyllbc_TransferPyError();
            return LLBC_FAILED;
        }
    }
    else if (type == PYLLBC_BYTEARRAY_OBJ)
    {
        buf = PyByteArray_AS_STRING(raw);
        len = PyByteArray_GET_SIZE(raw);
    }
    else if (type == PYLLBC_BUFFER_OBJ)
    {
        const void *cbuf = nullptr;
        if (PyObject_AsReadBuffer(raw, &cbuf, &len) != 0)
        {
            pyllbc_TransferPyError();
            return LLBC_FAILED;
        }

        buf = reinterpret_cast<char *>(const_cast<void *>(cbuf));
    }
    else
    {
        pyllbc_SetError("raw type must str/bytearray/buffer type", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _stream.SetPos(0);
    _stream.Write(buf, len);

    return LLBC_OK;
}

PyObject *pyllbc_Stream::Read(PyObject *cls)
{
    typedef pyllbc_TypeDetector _Detector;
    switch(_Detector::DetectCls(cls))
    {
    case PYLLBC_NONE_OBJ:
        return ReadNone();

    case PYLLBC_BOOL_OBJ:
        return ReadBool();

    case PYLLBC_INT_OBJ:
        return ReadInt32();

    case PYLLBC_LONG_OBJ:
        return ReadInt64();

    case PYLLBC_FLOAT_OBJ:
        return ReadDouble();

    case PYLLBC_STR_OBJ:
        return ReadStr();

    case PYLLBC_UNICODE_OBJ:
        return ReadUnicode();

    case PYLLBC_BYTEARRAY_OBJ:
        return ReadByteArray();

    case PYLLBC_BUFFER_OBJ:
        return ReadBuffer();

    case PYLLBC_TUPLE_OBJ:
    case PYLLBC_LIST_OBJ:
        pyllbc_SetError("not support to read 'tuple/list' type data", LLBC_ERROR_NOT_IMPL);
        return nullptr;

    case PYLLBC_DICT_OBJ:
        pyllbc_SetError("not support to read 'dict' type data", LLBC_ERROR_NOT_IMPL);
        return nullptr;

    default:
        break;
    }

    if (!PyObject_HasAttr(cls, _methDecode))
    {
        pyllbc_SetError("will decode class not exist 'decode' method", LLBC_ERROR_NOT_FOUND);
        return nullptr;
    }

    PyObject *obj = PyObject_CallObject(cls, nullptr);
    if (!obj)
    {
        PyObject *pyClsStr = PyObject_Str(cls);
        const LLBC_String clsStr = PyString_AS_STRING(pyClsStr);
        Py_DECREF(pyClsStr);

        LLBC_String addiMsg;
        pyllbc_TransferPyError(addiMsg.format(
            "When create class[%s] instance in Stream.unpack() method", clsStr.c_str()));
        return nullptr;
    }

    PyObject *rtn = PyObject_CallMethodObjArgs(obj, _methDecode, _pyStream, nullptr);
    if (!rtn)
    {
        pyllbc_TransferPyError();
        Py_DECREF(obj);

        return nullptr;
    }

    Py_DECREF(rtn);

    return obj;
}

PyObject *pyllbc_Stream::ReadNone()
{
    Py_RETURN_NONE;
}

PyObject *pyllbc_Stream::ReadByte()
{
    sint8 val;
    if (!_stream.Read(val))
    {
        pyllbc_SetError("not enough bytes to decode 'byte'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    return PyString_FromStringAndSize(&val, sizeof(sint8));
}

PyObject *pyllbc_Stream::ReadBool()
{
    bool val;
    if (!_stream.Read(val))
    {
        pyllbc_SetError("not enough bytes to decode'bool'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    PyObject *pyVal = val ? Py_True : Py_False;
    Py_INCREF(pyVal);

    return pyVal;
}

PyObject *pyllbc_Stream::ReadInt16()
{
    sint16 val;
    if (!_stream.Read(val))
    {
        pyllbc_SetError("not enough bytes to decode 'int16'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    return PyInt_FromLong(val);
}

PyObject *pyllbc_Stream::ReadInt32()
{
    sint32 val;
    if (!_stream.Read(val))
    {
        pyllbc_SetError("not enough bytes to decode 'int32'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    return PyInt_FromLong(val);
}

PyObject *pyllbc_Stream::ReadInt64()
{
    sint64 val;
    if (!_stream.Read(val))
    {
        pyllbc_SetError("not enough bytes to decode 'int64'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    return Py_BuildValue("L", val);
}

PyObject *pyllbc_Stream::ReadFloat()
{
    float val;
    if (!_stream.Read(val))
    {
        pyllbc_SetError("not enough bytes to decode 'float'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    return PyFloat_FromDouble(val);
}

PyObject *pyllbc_Stream::ReadDouble()
{
    double val;
    if (!_stream.Read(val))
    {
        pyllbc_SetError("not enough bytes to decode 'double'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    return PyFloat_FromDouble(val);
}


PyObject *pyllbc_Stream::ReadPyInt()
{
    return ReadInt64();
}

PyObject *pyllbc_Stream::ReadPyLong()
{
    return ReadInt64();
}

PyObject *pyllbc_Stream::ReadStr()
{
    auto pos = _stream.GetPos();
    const auto size = _stream.GetCap();
    if (UNLIKELY(pos == size))
        return PyString_FromStringAndSize("", 0);

    const char *buf = reinterpret_cast<const char *>(_stream.GetBuf());
    for (auto i = pos; i < size; ++i)
    {
        if (buf[i] == '\0')
        {
            const auto len = i - pos;
            PyObject *pyStr = PyString_FromStringAndSize(buf + pos, len);
            _stream.Skip(static_cast<long>(len + 1));

            return pyStr;
        }
    }

    PyObject *pyStr = PyString_FromStringAndSize(buf + pos, size - pos);
    _stream.SetPos(size);

    return pyStr;
}

PyObject *pyllbc_Stream::ReadStr2()
{
    uint32 len;
    if (!_stream.Read(len))
    {
        pyllbc_SetError("not enough bytes to decode 'str' len part", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    if (static_cast<uint32>(_stream.GetCap() - _stream.GetPos()) < len)
    {
        pyllbc_SetError("not enough bytes to decode 'str'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    PyObject *pyStr = PyString_FromStringAndSize(_stream.GetBufStartWithPos<char>(), len);
    _stream.Skip(len);

    return pyStr;
}

PyObject *pyllbc_Stream::ReadUnicode()
{
    uint32 len;
    if (!_stream.Read(len))
    {
        pyllbc_SetError("not enough bytes to decode 'unicode' len part", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    if (static_cast<uint32>(_stream.GetCap() - _stream.GetPos()) < len)
    {
        pyllbc_SetError("not enough bytes to decode 'unicode'", LLBC_ERROR_LIMIT);
        return nullptr;
    }

    PyObject *pyUni = PyUnicode_FromStringAndSize(_stream.GetBufStartWithPos<char>(), len);
    _stream.Skip(len);

    return pyUni;
}

PyObject *pyllbc_Stream::ReadByteArray()
{
    static const char *errStr = "not enough bytes to decode 'bytearray'";

    sint32 len;
    if (!_stream.Read(len))
    {
        pyllbc_SetError(errStr, LLBC_ERROR_LIMIT);
        return nullptr;
    }

    char *buf = LLBC_Malloc(char, len);
    if (!_stream.Read(buf, len))
    {
        pyllbc_SetError(errStr, LLBC_ERROR_LIMIT);
        free(buf);

        return nullptr;
    }

    PyObject *pyVal = 
        PyByteArray_FromStringAndSize(buf, len);
    free(buf);

    return pyVal;
}

PyObject *pyllbc_Stream::ReadBuffer()
{
    static const char *errStr = "not enough bytes to decode 'buffer'";

    sint32 len;
    if (!_stream.Read(len))
    {
        pyllbc_SetError(errStr, LLBC_ERROR_LIMIT);
        return nullptr;
    }

    PyObject *pyVal = PyBuffer_New(len);
    if (!pyVal)
    {
        pyllbc_TransferPyError();
        return nullptr;
    }

    void *buf;
    Py_ssize_t bufLen;
    if (PyObject_AsWriteBuffer(pyVal, &buf, &bufLen) != 0)
    {
        Py_DECREF(pyVal);
        pyllbc_TransferPyError();

        return nullptr;
    }

    if (!_stream.Read(buf, bufLen))
    {
        pyllbc_SetError(errStr, LLBC_ERROR_LIMIT);

        Py_DECREF(pyVal);
        return nullptr;
    }

    return pyVal;
}

PyObject *pyllbc_Stream::FmtRead(const LLBC_String &fmt, PyObject *callerEnv)
{
    pyllbc_PackLemma *lemma = 
        pyllbc_s_PackLemmaCompiler->Compile(fmt, false, callerEnv);
    if (!lemma)
        return nullptr;

    return lemma->Read(this);
}

int pyllbc_Stream::Write(PyObject *obj)
{
    typedef pyllbc_TypeDetector _Detector;
    switch (_Detector::Detect(obj))
    {
    case PYLLBC_NONE_OBJ:
        return WriteNone(obj);

    case PYLLBC_BOOL_OBJ:
        return WriteBool(obj);

    case PYLLBC_INT_OBJ:
        return WriteInt32(obj);

    case PYLLBC_LONG_OBJ:
        return WriteInt64(obj);

    case PYLLBC_FLOAT_OBJ:
        return WriteDouble(obj);

    case PYLLBC_STR_OBJ:
        return WriteStr(obj);

    case PYLLBC_UNICODE_OBJ:
        return WriteUnicode(obj);

    case PYLLBC_BYTEARRAY_OBJ:
        return WriteByteArray(obj);

    case PYLLBC_BUFFER_OBJ:
        return WriteBuffer(obj);

    case PYLLBC_TUPLE_OBJ:
        return WriteTuple(obj);

    case PYLLBC_LIST_OBJ:
        return WriteList(obj);

    case PYLLBC_SEQ_OBJ:
        return WriteSequence(obj);

    case PYLLBC_DICT_OBJ:
        return WriteDict(obj);

    default:
        break;
    }

    return WriteInst(obj);
}

int pyllbc_Stream::WriteNone(PyObject *val)
{
    return LLBC_OK;
}

int pyllbc_Stream::WriteByte(PyObject *val)
{
    if (pyllbc_TypeDetector::IsStr(val))
    {
        char *str;
        Py_ssize_t strLen;
        if (PyString_AsStringAndSize(val, &str, &strLen) == -1)
        {
            pyllbc_TransferPyError();
            return LLBC_FAILED;
        }
        
        if (strLen < 1)
        {
            pyllbc_SetError("could not write byte, len(str) < 1", LLBC_ERROR_LIMIT);
            return LLBC_FAILED;
        }

        _stream.Write(str, 1);

        return LLBC_OK;
    }
    else if (pyllbc_TypeDetector::IsByteArray(val))
    {
        const void *buf;
        Py_ssize_t  bufLen;
        if (PyObject_AsReadBuffer(val, &buf, &bufLen) != 0)
        {
            pyllbc_TransferPyError();
            return LLBC_FAILED;
        }

        if (bufLen < 1)
        {
            pyllbc_SetError("could not write byte, len(bytearray) < 1", LLBC_ERROR_LIMIT);
            return LLBC_FAILED;
        }

        _stream.Write(buf, 1);

        return LLBC_OK;
    }
    else if (pyllbc_TypeDetector::IsBuffer(val))
    {
        Py_buffer buffer;
        if (PyObject_GetBuffer(val, &buffer, PyBUF_SIMPLE) != 0)
        {
            pyllbc_TransferPyError();
            return LLBC_FAILED;
        }

        if (buffer.len < 1)
        {
            pyllbc_SetError("cound not write byte, len(buffer) < 1", LLBC_ERROR_LIMIT);
            PyBuffer_Release(&buffer);

            return LLBC_FAILED;
        }

        _stream.Write(buffer.buf, 1);
        PyBuffer_Release(&buffer);

        return LLBC_OK;
    }
    else if (pyllbc_TypeDetector::IsBool(val))
    {
        const bool boolVal = !!PyObject_IsTrue(val);
        _stream.Write(boolVal);

        return LLBC_OK;
    }
    else if (pyllbc_TypeDetector::IsInt(val) ||
        pyllbc_TypeDetector::IsLong(val))
    {
        const long longVal = PyLong_AsLong(val);
        _stream.Write(&longVal, 1);

        return LLBC_OK;
    }

    pyllbc_SetError("not support write byte type object(must str/bytearray/buffer/bool/int/long)", LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

int pyllbc_Stream::WriteBool(PyObject *val)
{
    bool bVal = !!PyObject_IsTrue(val);
    _stream.Write(bVal);

    return LLBC_OK;
}

int pyllbc_Stream::WriteInt16(PyObject *val)
{
    long lVal = PyInt_AsLong(val);
    if (lVal == -1 && PyErr_Occurred())
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    _stream.Write(static_cast<sint16>(lVal));

    return LLBC_OK;
}

int pyllbc_Stream::WriteInt32(PyObject *val)
{
    long lVal = PyInt_AsLong(val);
    if (lVal == -1 && PyErr_Occurred())
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    _stream.Write(static_cast<sint32>(lVal));

    return LLBC_OK;
}

int pyllbc_Stream::WriteInt64(PyObject *val)
{
    sint64 s64Val;
    if (!PyArg_Parse(val, "L", &s64Val))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    _stream.Write(s64Val);

    return LLBC_OK;
}

int pyllbc_Stream::WriteFloat(PyObject *val)
{
    double dbVal;
    if (!PyArg_Parse(val, "d", &dbVal))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    _stream.Write(static_cast<float>(dbVal));

    return LLBC_OK;
}

int pyllbc_Stream::WriteDouble(PyObject *val)
{
    double dbVal;
    if (!PyArg_Parse(val, "d", &dbVal))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    _stream.Write(dbVal);

    return LLBC_OK;
}

int pyllbc_Stream::WritePyInt(PyObject *val)
{
    return WriteInt64(val);
}

int pyllbc_Stream::WritePyLong(PyObject *val)
{
    return WriteInt64(val);
}

int pyllbc_Stream::WriteStr(PyObject *val)
{
    char *str;
    Py_ssize_t len;
    if (PyString_AsStringAndSize(val, &str, &len) == -1)
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    if (str && len > 0)
        _stream.Write(str, len);
    _stream.Write('\0');

    return LLBC_OK;
}

int pyllbc_Stream::WriteStr2(PyObject *val)
{
    char *str;
    Py_ssize_t len;
    if (PyString_AsStringAndSize(val, &str, &len) == -1)
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    _stream.Write(static_cast<uint32>(len));
    _stream.Write(str, len);

    return LLBC_OK;
}

int pyllbc_Stream::WriteUnicode(PyObject *val)
{
    PyObject *str = PyUnicode_AsUTF8String(val);
    if (!str)
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    const int rtn = WriteStr2(str);
    Py_DECREF(str);

    return rtn;
}

int pyllbc_Stream::WriteByteArray(PyObject *val)
{
    const void *buf = PyByteArray_AS_STRING(val);
    const Py_ssize_t len = PyByteArray_GET_SIZE(val);

    _stream.Write(static_cast<sint32>(len));
    if (len > 0)
        _stream.Write(buf, len);

    return LLBC_OK;
}

int pyllbc_Stream::WriteBuffer(PyObject *val)
{
    const void *buf;
    Py_ssize_t len;
    if (PyObject_AsReadBuffer(val, &buf, &len) != 0)
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    _stream.Write(static_cast<sint32>(len));
    if (len > 0)
        _stream.Write(buf, len);

    return LLBC_OK;
}

int pyllbc_Stream::WriteTuple(PyObject *val)
{
    return WriteSequence(val);
}

int pyllbc_Stream::WriteList(PyObject *val)
{
    return WriteSequence(val);
};

int pyllbc_Stream::WriteSequence(PyObject *val)
{
    const Py_ssize_t len = PySequence_Size(val);

    _stream.Write(static_cast<sint32>(len));

    for (Py_ssize_t i = 0; i < len; ++i)
    {
        PyObject *elem = PySequence_ITEM(val, i);
        if (Write(elem) != LLBC_OK)
        {
            Py_DECREF(elem);
            return LLBC_FAILED;
        }

        Py_DECREF(elem);
    }

    return LLBC_OK;
}

int pyllbc_Stream::WriteDict(PyObject *val)
{
    const Py_ssize_t len = PyDict_Size(val);
    
    _stream.Write(static_cast<sint32>(len));

    Py_ssize_t pos = 0;
    PyObject *key, *dictVal;
    while (PyDict_Next(val, &pos, &key, &dictVal))
    {
        if (WriteStr(key) != LLBC_OK)
            return LLBC_FAILED;

        if (Write(dictVal) != LLBC_OK)
            return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_Stream::WriteInst(PyObject *val)
{
    // 1) Search encode() method.
    if (PyObject_HasAttr(val, _methEncode))
    {
        PyObject *rtn = PyObject_CallMethodObjArgs(val, _methEncode, _pyStream, nullptr);
        if (!rtn)
        {
            pyllbc_TransferPyError();
            return LLBC_FAILED;
        }

        Py_DECREF(rtn);

        return LLBC_OK;
    }

    // 2) Use __dict__.
    PyObject *dict = PyObject_GetAttr(val, This::_keyDict);
    if (dict)
    {
        const int rtn = This::WriteDict(dict);
        Py_DECREF(dict);

        return rtn;
    }

    // 3) Use __slots__.
    PyObject *slots = PyObject_GetAttr(val, This::_keySlots);
    if (!slots)
    {
        pyllbc_SetError("could not write object", PYLLBC_ERROR_COMMON);
        return LLBC_FAILED;
    }

    std::vector<std::pair<PyObject *, PyObject *> > kws;

    const Py_ssize_t len = PySequence_Size(slots);
    for (Py_ssize_t i = 0; i < len; ++i)
    {
        PyObject *slotItem = PySequence_ITEM(slots, i);
        PyObject *slotVal = PyObject_GetAttr(val, slotItem);
        if (slotVal)
            kws.push_back(std::make_pair(slotItem, slotVal));
        else
            Py_DECREF(slotItem);
    }

    int rtn = LLBC_OK;
    _stream.Write(static_cast<sint32>(kws.size()));
    for (size_t i = 0; i < kws.size(); ++i)
    {
        if ((rtn = Write(kws[i].first)) != LLBC_OK)
            break;
        if ((rtn = Write(kws[i].second)) != LLBC_OK)
            break;
    }

    for (size_t i = 0; i < kws.size(); ++i)
    {
        Py_DECREF(kws[i].first);
        Py_DECREF(kws[i].second);
    }

    Py_DECREF(slots);

    return rtn;
}

int pyllbc_Stream::FmtWrite(const LLBC_String &fmt, PyObject *values, PyObject *callerEnv)
{
    pyllbc_PackLemma *lemma =
        pyllbc_s_PackLemmaCompiler->Compile(fmt, false, callerEnv);
    if (!lemma)
        return LLBC_FAILED;

    return lemma->Write(this, values);
}
