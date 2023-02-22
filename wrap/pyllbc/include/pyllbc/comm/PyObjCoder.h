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

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * \brief The python object coder class encapsulation.
 */
class LLBC_HIDDEN pyllbc_ObjCoder
{
public:
    /**
     * Encode python object.
     * @param[in] in   - will encode python object.
     * @param[out] out - encoded python object(json format).
     * @return int - return 0 if success, otherwise return -1.
     */
    static int Encode(PyObject *in, std::string &out);

    /** 
     * Decode python object from json format.
     * @param[in] in   - will decode json string.
     * @param[out] out - decoded python object.
     * @return int - return 0 if success, otherwise return -1.
      */
    static int Decode(const std::string &in, PyObject *&out);

private:
    static int Encode(PyObject *in, LLBC_Json::Document *&out);
    static int EncodeBool(PyObject *in, bool &out);
    static int EncodeInt(PyObject *in, sint64 &out);
    static int EncodeLong(PyObject *in, sint64 &out);
    static int EncodeFloat(PyObject *in, double &out);
    static int EncodeStr(PyObject *in, std::string &out);
    static int EncodeSeq(PyObject *in, LLBC_Json::Document &out);
    static int EncodeDict(PyObject *in, LLBC_Json::Document &out);
    static int EncodeInst(PyObject *in, LLBC_Json::Document &out);

    static int _Decode(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeNull(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeBool(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeInt(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeUInt(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeLong(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeULong(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeReal(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeStr(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeArray(const LLBC_JsonValue &in, PyObject *&out);
    static int DecodeDict(PyObject *in, LLBC_JsonValue *&out);
    static int DecodeObj(const LLBC_JsonValue &in, PyObject *&out);
};
