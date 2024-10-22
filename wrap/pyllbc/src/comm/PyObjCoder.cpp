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

#include "pyllbc/comm/PyObjCoder.h"

namespace
{
    typedef pyllbc_ObjCoder This;

    static LLBC_Json::StringBuffer buffer;
    static LLBC_Json::Writer<LLBC_Json::StringBuffer> writer(buffer);
}

int pyllbc_ObjCoder::Encode(PyObject *in, std::string &out)
{
    LLBC_Json::Document wrapJson(LLBC_Json::kArrayType);
    LLBC_Json::Document *jsonVal = new LLBC_Json::Document(&wrapJson.GetAllocator());
    if (This::Encode(in, jsonVal) != LLBC_OK)
    {
        delete jsonVal;
        return LLBC_FAILED;
    }

    wrapJson.PushBack((*jsonVal), wrapJson.GetAllocator());
    delete jsonVal;

    // json stringify
    wrapJson.Accept(writer);
    out = buffer.GetString();

    // clear that be used for reuse buffer and writer
    buffer.Clear();
    writer.Reset(buffer);

    return LLBC_OK;
}

int pyllbc_ObjCoder::Decode(const std::string &in, PyObject *&out)
{
    LLBC_Json::Document j;
    if (j.Parse(in.c_str()).HasParseError())
    {
        LLBC_String err;
        err.format("could not decode json string, err:%s", 
            LLBC_Json::GetParseError_En(j.GetParseError()), LLBC_ERROR_FORMAT);
        pyllbc_SetError(err);

        return LLBC_FAILED;
    }

    if (j.GetType() != LLBC_Json::kArrayType)
    {
        pyllbc_SetError("top json type not array", LLBC_ERROR_FORMAT);
        return LLBC_FAILED;
    }
    else if (j.Size() < 1)
    {
        pyllbc_SetError("top json size lt 1", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return This::_Decode(j[0u], out);
}

int pyllbc_ObjCoder::Encode(PyObject *in, LLBC_Json::Document *&out)
{
    int rtn = LLBC_FAILED;
    switch (pyllbc_TypeDetector::Detect(in))
    {
    case PYLLBC_NONE_OBJ:
        {
            rtn = LLBC_OK;
            out->SetNull();
        }
        break;

    case PYLLBC_BOOL_OBJ:
        {
            bool val;
            if ((rtn = This::EncodeBool(in, val)) != LLBC_OK)
                break;

            out->SetBool(val);
        }
        break;

    case PYLLBC_INT_OBJ:
    case PYLLBC_LONG_OBJ:
        {
            sint64 val;
            if ((rtn = This::EncodeInt(in, val)) != LLBC_OK)
                break;

            if (INT_MIN <= val && val <= INT_MAX)
                out->SetInt(static_cast<sint32>(val));
            else
                out->SetInt64(val);
        }
        break;

    case PYLLBC_FLOAT_OBJ:
        {
            double val;
            if ((rtn = This::EncodeFloat(in, val)) != LLBC_OK)
                break;

            out->SetDouble(val);
        }
        break;

    case PYLLBC_STR_OBJ:
        {
            std::string val;
            if ((rtn = This::EncodeStr(in, val)) != LLBC_OK)
                break;

            out->SetString(val.c_str(), out->GetAllocator());
        }
        break;

    case PYLLBC_UNICODE_OBJ:
        pyllbc_SetError("not support encode unicode object", LLBC_ERROR_NOT_IMPL);
        break;

    case PYLLBC_BUFFER_OBJ:
    case PYLLBC_BYTEARRAY_OBJ:
        pyllbc_SetError("not support encode bytearray/buffer object", LLBC_ERROR_NOT_IMPL);
        break;

    case PYLLBC_TUPLE_OBJ:
    case PYLLBC_LIST_OBJ:
    case PYLLBC_SEQ_OBJ:
        {
            out->SetArray();
            rtn = This::EncodeSeq(in, *out);
        }
        break;

    case PYLLBC_DICT_OBJ:
        {
            out->SetObject();
            rtn = This::EncodeDict(in, *out);
        }
        break;

    case PYLLBC_UNKNOWN_OBJ:
        {
            out->SetObject();
            rtn = This::EncodeInst(in, *out);
        }
        break;

    default:
        pyllbc_SetError("unknown object type", LLBC_ERROR_UNKNOWN);
        break;
    }

    return rtn;
}

int pyllbc_ObjCoder::EncodeBool(PyObject *in, bool &out)
{
    int iVal;
    if (!PyArg_Parse(in, "i", &iVal))
    {
        pyllbc_TransferPyError("when encoding bool object");
        return LLBC_FAILED;
    }

    out = iVal ? true : false;

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeInt(PyObject *in, sint64 &out)
{
    return This::EncodeLong(in, out);
}

int pyllbc_ObjCoder::EncodeLong(PyObject *in, sint64 &out)
{
    if (!PyArg_Parse(in, "L", &out))
    {
        pyllbc_TransferPyError("when encoding long object");
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeFloat(PyObject *in, double &out)
{
    if (!PyArg_Parse(in, "d", &out))
    {
        pyllbc_TransferPyError("when encoding float object");
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeStr(PyObject *in, std::string &out)
{
    char *s;
    if (!PyArg_Parse(in, "s", &s))
    {
        pyllbc_TransferPyError("when encoding str obj");
        return LLBC_FAILED;
    }

    out.append(s);
    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeSeq(PyObject *in, LLBC_Json::Document &out)
{
    PyObject *fastSeq = PySequence_Fast(in, "could not convert seq to FAST seq");
    if (UNLIKELY(!fastSeq))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    const Py_ssize_t len = PySequence_Fast_GET_SIZE(fastSeq);
    ASSERT(len >= 0 && "python API internal error");

    for (Py_ssize_t i = 0; i < len; ++i)
    {
        PyObject *elemObj = PySequence_Fast_GET_ITEM(fastSeq, i);
        if (UNLIKELY(!elemObj))
        {
            Py_DECREF(fastSeq);
            pyllbc_TransferPyError();

            return LLBC_FAILED;
        }

        LLBC_Json::Document *elemJson = new LLBC_Json::Document(&out.GetAllocator());
        if (This::Encode(elemObj, elemJson) != LLBC_OK)
        {
            Py_DECREF(fastSeq);
            delete elemJson;
            return LLBC_FAILED;
        }

        out.PushBack(*elemJson, out.GetAllocator());

        delete elemJson;
    }

    Py_DECREF(fastSeq);

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeDict(PyObject *in, LLBC_Json::Document &out)
{
    Py_ssize_t pos = 0;
    PyObject *key, *val;
    while (PyDict_Next(in, &pos, &key, &val))
    {
        char *strKey;
        if (!PyArg_Parse(key, "s", &strKey))
        {
            pyllbc_TransferPyError("when encoding dict obj");
            return LLBC_FAILED;
        }

        LLBC_Json::Document *valJson = new LLBC_Json::Document(&out.GetAllocator());
        if (This::Encode(val, valJson) != LLBC_OK)
        {
            delete valJson;
            return LLBC_FAILED;
        }

        out.AddMember(LLBC_JsonValue(strKey, out.GetAllocator()), *valJson, out.GetAllocator());

        delete valJson;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeInst(PyObject *in, LLBC_Json::Document &out)
{
    PyObject *dict = PyObject_GetAttrString(in, "__dict__");
    if (dict)
    {
        const int rtn = This::EncodeDict(dict, out);
        Py_DECREF(dict);

        return rtn;
    }
    
    PyObject *slots = PyObject_GetAttrString(in, "__slots__");
    if (!slots)
    {
        pyllbc_SetError("could not encode object", PYLLBC_ERROR_COMMON);
        return LLBC_FAILED;
    }

    const Py_ssize_t len = PySequence_Size(slots);
    for (Py_ssize_t i = 0; i < len; ++i)
    {
        PyObject *slotItem = PySequence_ITEM(slots, i);
        PyObject *slotVal = PyObject_GetAttr(in, slotItem);
        if (slotVal)
        {
            LLBC_Json::Document *slotValJson = new LLBC_Json::Document(&out.GetAllocator());
            if (This::Encode(slotVal, slotValJson) != LLBC_OK)
            {
                delete slotValJson;
                Py_DECREF(slotVal);
                Py_DECREF(slotItem);

                Py_DECREF(slots);

                return LLBC_FAILED;
            }

            char *itemStr;
            if (UNLIKELY(!PyArg_Parse(slotItem, "s", &itemStr)))
            {
                pyllbc_TransferPyError();

                delete slotValJson;

                Py_DECREF(slotVal);
                Py_DECREF(slotItem);

                Py_DECREF(slots);

                return LLBC_FAILED;
            }

            out.AddMember(LLBC_JsonValue(itemStr, out.GetAllocator()), *slotValJson, out.GetAllocator());

            delete slotValJson;

            Py_DECREF(slotVal);
        }

        Py_DECREF(slotItem);
    }

    Py_DECREF(slots);

    return LLBC_OK;
}

int pyllbc_ObjCoder::_Decode(const LLBC_JsonValue &in, PyObject *&out)
{
    int rtn = LLBC_OK;
    switch(in.GetType())
    {
    case LLBC_Json::kNullType:
        rtn = This::DecodeNull(in, out);
        break;

    case  LLBC_Json::kFalseType:
        rtn = This::DecodeBool(in, out);
        break;

    case  LLBC_Json::kTrueType:
        rtn = This::DecodeBool(in, out);
        break;

    case LLBC_Json::kNumberType:
        if (in.IsDouble())         rtn = This::DecodeReal(in, out);
        else if (in.IsInt())       rtn = This::DecodeInt(in, out);
        else if (in.IsUint())      rtn = This::DecodeUInt(in, out);
        else if (in.IsInt64())     rtn = This::DecodeLong(in, out);
        else                       rtn = This::DecodeULong(in, out);
        break;

    case LLBC_Json::kStringType:
        rtn = This::DecodeStr(in, out);
        break;

    case LLBC_Json::kArrayType:
        rtn = This::DecodeArray(in, out);
        break;

    case LLBC_Json::kObjectType:
        rtn = This::DecodeObj(in, out);
        break;

    default:
        pyllbc_SetError("not support json object type", LLBC_ERROR_NOT_IMPL);
        rtn = LLBC_FAILED;
        break;
    }

    return rtn;
}

int pyllbc_ObjCoder::DecodeNull(const LLBC_JsonValue &in, PyObject *&out)
{
    Py_INCREF(Py_None);
    out = Py_None;

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeBool(const LLBC_JsonValue &in, PyObject *&out)
{
    const bool boolVal = in.GetBool();

    out = boolVal ? Py_True : Py_False;
    Py_INCREF(out);

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeInt(const LLBC_JsonValue &in, PyObject *&out)
{
    if(!(out = Py_BuildValue("i", in.GetInt())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeUInt(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("I", in.GetUint())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeLong(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("L", in.GetInt64())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeULong(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("K", in.GetUint64())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeReal(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("d", in.GetDouble())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeStr(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("s", in.GetString())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeArray(const LLBC_JsonValue &in, PyObject *&out)
{
    out = PyTuple_New(in.Size());
    if (UNLIKELY(!out))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    Py_ssize_t tupleIdx = 0;
    for (LLBC_JsonValueCIter it = in.Begin();
         it != in.End();
         it++)
    {
        const LLBC_JsonValue &elemJson = *it;

        PyObject *elemObj = nullptr;
        if (This::_Decode(elemJson, elemObj) != LLBC_OK)
        {
            Py_DECREF(out);
            out = nullptr;

            return LLBC_FAILED;
        }

        if (UNLIKELY(PyTuple_SetItem(out, tupleIdx++, elemObj) != 0))
        {
            pyllbc_TransferPyError();

            Py_DECREF(elemObj);

            Py_DECREF(out);
            out = nullptr;

            return LLBC_FAILED;
        }
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeObj(const LLBC_JsonValue &in, PyObject *&out)
{
    if (UNLIKELY(!(out = PyDict_New())))
    {
        LLBC_SetLastError(PYLLBC_ERROR_COMMON);
        return LLBC_FAILED;
    }

    for (LLBC_JsonMemberCIter it = in.MemberBegin();
         it != in.MemberEnd();
         it++)
    {
        const LLBC_JsonValue &key = it->name;
        if (UNLIKELY(key.GetType() != LLBC_Json::kStringType))
        {
            pyllbc_SetError("dict key not string type", LLBC_ERROR_INVALID);

            Py_DECREF(out);
            out = nullptr;

            return LLBC_FAILED;
        }

        PyObject *elemObj = nullptr;
        const LLBC_JsonValue &elemJson = it->value;
        if (This::_Decode(elemJson, elemObj) != LLBC_OK)
        {
            Py_DECREF(out);
            out = nullptr;

            return LLBC_FAILED;
        }

        if (UNLIKELY(PyDict_SetItemString(out, it->name.GetString(), elemObj) != 0))
        {
            pyllbc_TransferPyError();
            
            Py_DECREF(elemObj);

            Py_DECREF(out);
            out = nullptr;

            return LLBC_FAILED;
        }

        Py_DECREF(elemObj);
    }

    return LLBC_OK;
}
