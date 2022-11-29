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

LLBC_EXTERN_C PyObject *_pyllbc_NewPyStream(PyObject *self, PyObject *args)
{
    int initCap = 0;
    PyObject *pyStream;
    int endian = LLBC_MachineEndian;
    if (!PyArg_ParseTuple(args, "O|ii", &pyStream, &initCap, &endian))
        return nullptr;

    pyllbc_Stream *stream = new pyllbc_Stream(pyStream, initCap);
    if (UNLIKELY(stream->SetEndian(endian) != LLBC_OK))
    {
        delete stream;
        return nullptr;
    }

    return Py_BuildValue("l", stream);
}

LLBC_EXTERN_C PyObject *_pyllbc_DelPyStream(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    delete stream;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetPyStreamEndian(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return Py_BuildValue("i", stream->GetEndian());
};

LLBC_EXTERN_C PyObject *_pyllbc_SetPyStreamEndian(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    int endian;
    if (!PyArg_ParseTuple(args, "li", &stream, &endian))
        return nullptr;

    if (stream->SetEndian(endian) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetPyStreamPos(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return Py_BuildValue("I", stream->GetPos());
}

LLBC_EXTERN_C PyObject *_pyllbc_SetPyStreamPos(PyObject *self, PyObject *args)
{
    uint32 pos;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lI", &stream, &pos))
        return nullptr;
    
    if (stream->SetPos(pos) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_GetPyStreamCap(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return Py_BuildValue("I", stream->GetCap());
}

LLBC_EXTERN_C PyObject *_pyllbc_SetPyStreamCap(PyObject *self, PyObject *args)
{
    uint32 newCap;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lI", &stream, &newCap))
        return nullptr;

    if (stream->Recap(newCap) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamFmtRead(PyObject *self, PyObject *args)
{
    char *fmt;
    pyllbc_Stream *stream;
    PyObject *callerEnv;
    if (!PyArg_ParseTuple(args, "lsO", &stream, &fmt, &callerEnv))
        return nullptr;

    if (pyllbc_TypeDetector::IsNone(callerEnv))
        callerEnv = nullptr;

    return stream->FmtRead(fmt, callerEnv);
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead(PyObject *self, PyObject *args)
{
    PyObject *cls;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &cls))
        return nullptr;

    return stream->Read(cls);
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_None(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadNone();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Byte(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadByte();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Bool(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadBool();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Int16(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadInt16();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Int32(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadInt32();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Int64(PyObject *self ,PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadInt64();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Float(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadFloat();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Double(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadDouble();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_PyInt(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadPyInt();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_PyLong(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadPyLong();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Str(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadStr();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Str2(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadStr2();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Unicode(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadUnicode();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_ByteArray(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadByteArray();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Buffer(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->ReadBuffer();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamRead_Stream(PyObject *self, PyObject *args)
{
    pyllbc_Stream *willReadStream;
    int begin = 0, end = -1;
    if (!PyArg_ParseTuple(args, "l|ii", &willReadStream, &begin, &end))
        return nullptr;

    LLBC_Stream &llbcWillRead = willReadStream->GetLLBCStream();

    begin = MIN(MAX(0, begin), static_cast<int>(llbcWillRead.GetCap()));

    if (end == -1)
        end = static_cast<int>(llbcWillRead.GetCap());
    else
        end = MIN(MAX(0, end), static_cast<int>(llbcWillRead.GetCap()));

    // Get python layer stream class object.
    PyObject *streamCls = pyllbc_TopModule->GetObject("Stream"); // Borrow reference.

    // Construct callable arguments(tuple).
    PyObject *callArgs = PyTuple_New(1);
    PyTuple_SetItem(callArgs, 0, PyInt_FromLong(end - begin)); // Steals a reference to obj.
    
    // Create python layer stream object instance.
    PyObject *pyStream = PyObject_CallObject(streamCls, callArgs);
    Py_DECREF(callArgs);

    if (UNLIKELY(!pyStream))
        return nullptr;

    // Get stream's cobj as pyllbc_Stream.
    PyObject *pyCObj = PyObject_GetAttrString(pyStream, "cobj"); // New reference.
    if (UNLIKELY(!pyCObj))
    {
        Py_DECREF(pyStream);
        return nullptr;
    }

    pyllbc_Stream *stream;
    if (!PyArg_Parse(pyCObj, "l", &stream))
    {
        Py_DECREF(pyStream);
        Py_DECREF(pyCObj);
        
        return nullptr;
    }

    Py_DECREF(pyCObj);

    // Get wrapped LLBC_Stream object, and write self stream data into it.
    LLBC_Stream &llbcStream = stream->GetLLBCStream();
    llbcStream.Write(reinterpret_cast<
        const char *>(llbcWillRead.GetBuf()) + begin, end - begin);

    return pyStream;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamFmtWrite(PyObject *self, PyObject *args)
{
    char *fmt;
    PyObject *values;
    pyllbc_Stream *stream;
    PyObject *callerEnv;
    if (!PyArg_ParseTuple(args, "lsOO", &stream, &fmt, &values, &callerEnv))
        return nullptr;

    if (pyllbc_TypeDetector::IsNone(callerEnv))
        callerEnv = nullptr;


    if (stream->FmtWrite(fmt, values, callerEnv) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->Write(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_None(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteNone(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Byte(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteByte(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Bool(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteBool(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Int16(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteInt16(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Int32(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteInt32(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Int64(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteInt64(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Float(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteFloat(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Double(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteDouble(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_PyInt(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WritePyInt(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_PyLong(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WritePyLong(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Str(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteStr(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Str2(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteStr2(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Unicode(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteUnicode(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_ByteArray(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteByteArray(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Buffer(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteBuffer(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Tuple(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteTuple(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_List(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteList(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Sequence(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteSequence(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Dict(PyObject *self, PyObject *args)
{
    PyObject *obj;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &obj))
        return nullptr;

    if (stream->WriteDict(obj) != LLBC_OK)
        return nullptr;

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamWrite_Stream(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    pyllbc_Stream *willWrite;
    int beginPos = 0, endPos = -1;
    if (!PyArg_ParseTuple(args, "ll|ii", &stream, &willWrite, &beginPos, &endPos))
        return nullptr;

    beginPos = MIN(MAX(0, beginPos), static_cast<int>(willWrite->GetCap()));

    if (endPos == -1)
        endPos = static_cast<int>(willWrite->GetCap());
    else
        endPos = MIN(MAX(0, endPos), static_cast<int>(willWrite->GetCap()));

    if (beginPos > endPos)
        LLBC_Swap(beginPos, endPos);

    LLBC_Stream &llbcStream = stream->GetLLBCStream();
    LLBC_Stream &llbcWillWrite = willWrite->GetLLBCStream();
    llbcStream.Write(reinterpret_cast<
        const char *>(llbcWillWrite.GetBuf()) + beginPos, endPos - beginPos);

    return stream->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamGetRaw(PyObject *self, PyObject *args)
{
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "l", &stream))
        return nullptr;

    return stream->GetRaw();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamSetRaw(PyObject *self, PyObject *args)
{
    PyObject *raw;
    pyllbc_Stream *stream;
    if (!PyArg_ParseTuple(args, "lO", &stream, &raw))
        return nullptr;

    if (stream->SetRaw(raw) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamEncodeSelf(PyObject *self, PyObject *args)
{
    pyllbc_Stream *pyStream1, *pyStream2;
    if (!PyArg_ParseTuple(args, "ll", &pyStream1, &pyStream2))
        return nullptr;

    LLBC_Stream &s1 = pyStream1->GetLLBCStream();
    LLBC_Stream &s2 = pyStream2->GetLLBCStream();

    s2.Write(s1.GetBuf(), s1.GetPos());

    return pyStream1->GetPyObj();
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamGetCachedSize(PyObject *self, PyObject *args)
{
    const size_t size = pyllbc_s_PackLemmaCompiler->GetCachedSize();
#if LLBC_TARGET_PROCESSOR_X86_64
    return PyInt_FromLong(static_cast<long>(size));
#else
    return PyInt_FromLong(size);
#endif
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamIsExprCompiled(PyObject *self, PyObject *args)
{
    char *expr;
    if (!PyArg_ParseTuple(args, "s", &expr))
        return nullptr;

    if (pyllbc_s_PackLemmaCompiler->IsExprCompiled(expr))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamGetCacheLimit(PyObject *self, PyObject *args)
{
    const size_t lmt = pyllbc_s_PackLemmaCompiler->GetCacheLimit();
#if LLBC_TARGET_PROCESSOR_X86_64
    return PyInt_FromLong(static_cast<long>(lmt));
#else
    return PyInt_FromLong(lmt);
#endif
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamSetCacheLimit(PyObject *self, PyObject *args)
{
    LLBC_NS ulong cacheLimit;
    if (!PyArg_ParseTuple(args, "k", &cacheLimit))
        return nullptr;

    pyllbc_PackLemmaCompiler *compiler = pyllbc_s_PackLemmaCompiler;
    if (compiler->SetCacheLimit(cacheLimit) != LLBC_OK)
        return nullptr;

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamDiscardExpr(PyObject *self, PyObject *args)
{
    char *expr;
    if (!PyArg_ParseTuple(args, "s", &expr))
        return nullptr;

    pyllbc_s_PackLemmaCompiler->DiscardCache(expr);

    Py_RETURN_NONE;
}

LLBC_EXTERN_C PyObject *_pyllbc_PyStreamDiscardAllExprs(PyObject *self, PyObject *args)
{
    pyllbc_s_PackLemmaCompiler->DiscardAllCaches();

    Py_RETURN_NONE;
}
