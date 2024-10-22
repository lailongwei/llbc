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
#include "pyllbc/common/PackLemmaCompiler.h"
#include "pyllbc/common/PyStream.h"

LLBC_EXTERN_C PyObject *_pyllbc_Stream_New(PyObject *self, PyObject *args)
{
    PyObject *pyStream;
    int endian = LLBC_DefaultEndian;
    unsigned PY_LONG_LONG initCap = 0;
    if (!PyArg_ParseTuple(args, "O|Ki", &pyStream, &initCap, &endian))
        return nullptr;

    pyllbc_Stream *stream = new pyllbc_Stream(pyStream, static_cast<size_t>(initCap));
    if (UNLIKELY(stream->SetEndian(endian) != LLBC_OK))
    {
        delete stream;
        return nullptr;
    }

    return PyLong_FromVoidPtr(stream);
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Del(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    delete stream;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_GetEndian(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return PyInt_FromLong(stream->GetEndian());
};

LLBC_EXTERN_C PyObject *_pyllbc_Stream_SetEndian(PyObject *self, PyObject *args)
{
    int endian;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "i", &endian);

    if (stream->SetEndian(endian) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_GetReadPos(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return PyLong_FromSize_t(stream->GetReadPos());
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_SetReadPos(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    unsigned PY_LONG_LONG rpos;
    PYLLBC_ParseCObjBeginArgs(stream, "K", &rpos);

    if (stream->SetReadPos(static_cast<size_t>(rpos)) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_GetWritePos(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return PyLong_FromSize_t(stream->GetWritePos());
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_SetWritePos(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    unsigned PY_LONG_LONG wpos;
    PYLLBC_ParseCObjBeginArgs(stream, "K", &wpos);

    if (stream->SetWritePos(static_cast<size_t>(wpos)) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_GetCap(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream = nullptr;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return PyLong_FromSize_t(stream->GetCap());
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_SetCap(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    unsigned PY_LONG_LONG newCap;
    PYLLBC_ParseCObjBeginArgs(stream, "K", &newCap);

    if (stream->Recap(static_cast<size_t>(newCap)) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_GetRaw(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->GetRaw();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_SetRaw(PyObject *self, PyObject *args)
{
    PyObject *raw;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &raw);

    if (stream->SetRaw(raw) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_FmtRead(PyObject *self, PyObject *args)
{
    char *fmt;
    PyObject *callerEnv;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "sO", &fmt, &callerEnv);

    if (pyllbc_TypeDetector::IsNone(callerEnv))
        callerEnv = nullptr;

    return stream->FmtRead(fmt, callerEnv);
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read(PyObject *self, PyObject *args)
{
    PyObject *cls;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &cls);

    return stream->Read(cls);
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_None(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadNone();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Byte(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadByte();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Bool(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadBool();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Int16(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadInt16();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Int32(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadInt32();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Int64(PyObject *self ,PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadInt64();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Float(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadFloat();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Double(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadDouble();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Str(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadStr();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Str2(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
	PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadStr2();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Unicode(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadUnicode();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_ByteArray(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadByteArray();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Buffer(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "");

    return stream->ReadBuffer();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Read_Stream(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    int begin = 0, end = -1;
    PYLLBC_ParseCObjBeginArgs(stream, "|ii", &begin, &end);

    auto &llbcStream = stream->GetLLBCStream();

    begin = MIN(MAX(0, begin), static_cast<int>(llbcStream.GetWritePos()));
    if (end == -1)
        end = static_cast<int>(llbcStream.GetWritePos());
    else
        end = MIN(MAX(0, end), static_cast<int>(llbcStream.GetWritePos()));

    if (begin > end)
        std::swap(begin, end);

    // Get python layer stream class object.
    PyObject *streamCls = pyllbc_TopModule->GetObject("Stream"); // Borrow reference.

    // Construct callable arguments(tuple).
    const size_t newStreamCap = end - begin;
    PyObject *callArgs = PyTuple_New(1);
    PyTuple_SetItem(callArgs, 0, PyLong_FromUnsignedLongLong(newStreamCap)); // Steals a reference to obj.
    
    // Create python layer stream object instance.
    PyObject *newPyStream = PyObject_CallObject(streamCls, callArgs);
    Py_DECREF(callArgs);

    if (UNLIKELY(!newPyStream))
        return nullptr;

    // If newStreamCap is 0, return.
    if (newStreamCap == 0)
        return newPyStream;

    // Get stream's cobj as pyllbc_Stream.
    PyObject *newStreamCObj = PyObject_GetAttrString(newPyStream, "cobj"); // New reference.
    if (UNLIKELY(!newStreamCObj))
    {
        Py_DECREF(newPyStream);
        return nullptr;
    }

    pyllbc_Stream *newStream = reinterpret_cast<pyllbc_Stream *>(PyLong_AsVoidPtr(newStreamCObj));
    if (UNLIKELY(!newStream))
    {
        Py_DECREF(newStreamCObj);
        Py_DECREF(newPyStream);
        
        return nullptr;
    }

    Py_DECREF(newStreamCObj);

    // Get wrapped LLBC_Stream object, and write self stream data into it.
    LLBC_Stream &newLLBCStream = newStream->GetLLBCStream();
    newLLBCStream.Write(llbcStream.GetBuf<char>() + begin, newStreamCap);

    return newPyStream;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_FmtWrite(PyObject *self, PyObject *args)
{
    char *fmt;
    PyObject *values;
    PyObject *callerEnv;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "sOO", &fmt, &values, &callerEnv);

    if (pyllbc_TypeDetector::IsNone(callerEnv))
        callerEnv = nullptr;

    if (stream->FmtWrite(fmt, values, callerEnv) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->Write(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_None(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteNone(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Byte(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteByte(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Bool(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteBool(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Int16(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteInt16(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Int32(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteInt32(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Int64(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteInt64(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Float(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteFloat(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Double(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteDouble(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Str(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteStr(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Str2(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
	PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteStr2(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Unicode(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteUnicode(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_ByteArray(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteByteArray(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Buffer(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteBuffer(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Tuple(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteTuple(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_List(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteList(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Sequence(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteSequence(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Dict(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    PYLLBC_ParseCObjBeginArgs(stream, "O", &obj);

    if (stream->WriteDict(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_Write_Stream(PyObject *self, PyObject *args)
{
    int beginPos = 0, endPos = -1;
    unsigned PY_LONG_LONG toStreamPtrVal, fromStreamPtrVal;
    if (!PyArg_ParseTuple(args, "KK|ii", &toStreamPtrVal, &fromStreamPtrVal, &beginPos, &endPos))
        return nullptr;

    pyllbc_Stream *toStream, *fromStream;
    memcpy(&toStream, &toStreamPtrVal, sizeof(toStream));
    memcpy(&fromStream, &fromStreamPtrVal, sizeof(fromStream));

    beginPos = MIN(MAX(0, beginPos), static_cast<int>(fromStream->GetWritePos()));
    if (endPos == -1)
        endPos = static_cast<int>(fromStream->GetWritePos());
    else
        endPos = MIN(MAX(0, endPos), static_cast<int>(fromStream->GetWritePos()));

    if (beginPos > endPos)
        std::swap(beginPos, endPos);

    LLBC_Stream &toLLBCStream = toStream->GetLLBCStream();
    LLBC_Stream &fromLLBCStream = fromStream->GetLLBCStream();
    toLLBCStream.Write(fromLLBCStream.GetBuf<char>() + beginPos, endPos - beginPos);

    return toStream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_EncodeSelf(PyObject *self, PyObject *args)
{
    unsigned PY_LONG_LONG pyStream1PtrVal, pyStream2PtrVal;
    if (!PyArg_ParseTuple(args, "KK", &pyStream1PtrVal, &pyStream2PtrVal))
        return nullptr;

    pyllbc_Stream *pyStream1, *pyStream2;
    memcpy(&pyStream1, &pyStream1PtrVal, sizeof(pyStream1));
    memcpy(&pyStream2, &pyStream2PtrVal, sizeof(pyStream2));

    LLBC_Stream &llbcStream1 = pyStream1->GetLLBCStream();
    LLBC_Stream &llbcStream2 = pyStream2->GetLLBCStream();

    llbcStream2.Write(llbcStream1.GetBuf(), llbcStream1.GetWritePos());

    return pyStream1->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_GetCachedCompiledExprSize(PyObject *self, PyObject *args)
{
    const size_t size = pyllbc_s_PackLemmaCompiler->GetCachedSize();
    return PyLong_FromSize_t(size);
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_GetCompiledExprCacheLimit(PyObject *self, PyObject *args)
{
    const size_t lmt = pyllbc_s_PackLemmaCompiler->GetCacheLimit();
    return PyLong_FromSize_t(lmt);
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_SetCompiledExprCacheLimit(PyObject *self, PyObject *args)
{
    uint32 cacheLimit;
    if (!PyArg_ParseTuple(args, "I", &cacheLimit))
        return nullptr;

    pyllbc_PackLemmaCompiler *compiler = pyllbc_s_PackLemmaCompiler;
    if (compiler->SetCacheLimit(static_cast<size_t>(cacheLimit)) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_IsExprCompiled(PyObject *self, PyObject *args)
{
    char *expr;
    if (!PyArg_ParseTuple(args, "s", &expr))
        return nullptr;

    if (pyllbc_s_PackLemmaCompiler->IsExprCompiled(expr))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_UncacheCompiledExpr(PyObject *self, PyObject *args)
{
    char *expr;
    if (!PyArg_ParseTuple(args, "s", &expr))
        return nullptr;

    pyllbc_s_PackLemmaCompiler->DiscardCache(expr);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_Stream_UncacheAllCompiledExprs(PyObject *self, PyObject *args)
{
    pyllbc_s_PackLemmaCompiler->DiscardAllCaches();

    Py_RETURN_NONE;
}

