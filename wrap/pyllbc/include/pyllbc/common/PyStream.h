// The MIT License(MIT)

// Copyright(c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files(the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "pyllbc/common/LibHeader.h"

/**
 * \brief The python stream class c++ wrap class encapsulation.
 */
class LLBC_HIDDEN pyllbc_Stream
{
public:
    static const size_t npos = LLBC_Stream::npos;

public:
    /**
     * Parameter constructor.
     * @param[in] pyStream - the python stream(steal reference).
     * @param[in] cap      - the stream initialize capacity, default is 0.
     */
    pyllbc_Stream(PyObject *pyStream, size_t cap = 0);

    /**
     * Destructor.
     */
    ~pyllbc_Stream();

public:
    /**
     * Get stream endian flag.
     * @return int - the endian enumeration.
     */
    int GetEndian() const;

    /**
     * Set stream endian flag.
     * @param[in] endian - the endian enumeration.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetEndian(int endian);

public:
    /**
     * Get current position.
     * @return size_t - the position value.
     */
    size_t GetPos() const;

    /**
     * Set current position.
     * @param[in] pos - the new position.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetPos(size_t pos);

    /**
     * Get stream capacity.
     * return size_t - the capacity, in bytes.
     */
    size_t GetCap() const;

    /**
     * Recapacity stream(the new capacity must greater than old newCap).
     * @param[in] newCap - the new stream capacity.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Recap(size_t newCap);

public:
    /**
     * Get llbc library stream.
     * @return LLBC_Stream & - the llbc library stream.
     */
    LLBC_Stream &GetLLBCStream();

    /**
     * Get llbc library stream(const).
     * @return const LLBC_Stream & - the llbc library stream.
     */
    const LLBC_Stream &GetLLBCStream() const;

    /**
     * Get python object.
     * @return PyObject * - the python object(python layer class instance), new reference.
     */
    PyObject *GetPyObj();

    /**
     * Get RAW data.
     * @return PyObject * - the raw python object(new reference).
     */
    PyObject *GetRaw();

    /**
     * Set RAW data.
     * @param[in] raw - the raw python data(must be str/bytearray/buffer)(not steal reference, normal).
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetRaw(PyObject *raw);

public:

    /**
     * Read data.
     * @param[in] cls - class object(not steal reference, will increment it).
     * @return PyObject * - the readed instance object(new reference), return nullptr means failed.
     */
    PyObject *Read(PyObject *cls);
    PyObject *ReadNone();
    PyObject *ReadByte();
    PyObject *ReadBool();
    PyObject *ReadInt16();
    PyObject *ReadInt32();
    PyObject *ReadInt64();
    PyObject *ReadFloat();
    PyObject *ReadDouble();
    PyObject *ReadPyInt();
    PyObject *ReadPyLong();
    PyObject *ReadStr();
    PyObject *ReadStr2();
    PyObject *ReadUnicode();
    PyObject *ReadByteArray();
    PyObject *ReadBuffer();

    /**
     * Format read data.
     * @param[in] fmt       - the read format.
     * @param[in] callerEnv - the caller environment.
     * @return PyObject * - the result data. always is a tuple.
     * Note, the format table:
     *  c: char value(like b).
     *  b: byte value(like c).
     *  B: boolean value.
     *  s: short value.
     *  i: integer value.
     *  q: signed long long value.
     *  f: float value.
     *  d: double value(only support Fomat method).
     *  S: string value.
     *  S#:string value(use another coder algorithm, 4bytes length + string content(not include nullptr character).
     *  U: unicode value.
     *  A: byte array value.
     *  F: buffer value.
     *  N: None value.
     *  C: class type, will automatic call class.encode() method to decode.
     *     must tell stream this class name, using: C<ClassName> semantic.
     *  (): tuple type, if only has one element,  it represent tuple all element type is the given type, otherwise
     *      the tuple size must equal your given element count.
     *  []: list type, the same as tuple type: ().
     *  {key:value}: dictionary type.
     *  
     *  The format examples:
     *      iiS
     *      (i)
     *      (U)
     *      [i]
     *      {i:S}
     *      {i:(C<int>)}
     *      ([SC<int>NA(i)]{int:S}B
     */
    PyObject *FmtRead(const LLBC_String &fmt, PyObject *callerEnv = nullptr);

    /**
     * Write data to stream.
     * @param[in] obj/val - the python object/will write to stream RAW c/c++ object.
     *                      not steal reference, normal.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Write(PyObject *obj);
    int WriteNone(PyObject *val);
    int WriteByte(PyObject *val);
    int WriteBool(PyObject *val);
    int WriteInt16(PyObject *val);
    int WriteInt32(PyObject *val);
    int WriteInt64(PyObject *val);
    int WriteFloat(PyObject *val);
    int WriteDouble(PyObject *val);
    int WritePyInt(PyObject *val);
    int WritePyLong(PyObject *val);
    int WriteStr(PyObject *val);
    int WriteStr2(PyObject *val);
    int WriteUnicode(PyObject *val);
    int WriteByteArray(PyObject *val);
    int WriteBuffer(PyObject *val);
    int WriteTuple(PyObject *val);
    int WriteList(PyObject *val);
    int WriteSequence(PyObject *val);
    int WriteDict(PyObject *val);
    int WriteInst(PyObject *val);

    /**
     * Format write data.
     * @param[in] fmt       - the read format.
     * @param[in] values    - the values(must sequence, normal, not steal reference).
     * @param[in] callerEnv - the caller enviornments.
     * @return int - 0 if success, otherwise return -1.
     *               About the fmt characters decription, see FmtRead() method doc.
     */
    int FmtWrite(const LLBC_String &fmt, PyObject *values, PyObject *callerEnv = nullptr);

private:
    LLBC_Stream _stream;
    PyObject *_pyStream;

    static PyObject *_methEncode;
    static PyObject *_methDecode;

    static PyObject *_keyDict;
    static PyObject *_keySlots;
};

#include "pyllbc/common/PyStreamInl.h"


