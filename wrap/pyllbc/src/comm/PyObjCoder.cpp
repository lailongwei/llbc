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

    static Json::FastWriter __g_jsonFmter;
}

int pyllbc_ObjCoder::Encode(PyObject *in, std::string &out)
{
    LLBC_JsonValue *jsonVal = NULL;
    if (This::Encode(in, jsonVal) != LLBC_OK)
        return LLBC_FAILED;

    LLBC_JsonValue wrapJson(Json::arrayValue);

    wrapJson.append(*jsonVal);
    LLBC_Delete(jsonVal);

    out = __g_jsonFmter.write(wrapJson);

    return LLBC_OK;
}

int pyllbc_ObjCoder::Decode(const std::string &in, PyObject *&out)
{
    LLBC_JsonValue j;
    Json::Reader reader;
    if (!reader.parse(in, j))
    {
        LLBC_String err;
        err.format("could not decode json string, err:%s", 
            reader.getFormatedErrorMessages().c_str(), LLBC_ERROR_FORMAT);
        pyllbc_SetError(err);

        return LLBC_FAILED;
    }

    if (j.type() != Json::arrayValue)
    {
        pyllbc_SetError("top json type not array", LLBC_ERROR_FORMAT);
        return LLBC_FAILED;
    }
    else if (j.size() < 1)
    {
        pyllbc_SetError("top json size lt 1", LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return This::_Decode(j[0u], out);
}

int pyllbc_ObjCoder::Encode(PyObject *in, LLBC_JsonValue *&out)
{
    int rtn = LLBC_FAILED;
    switch (pyllbc_TypeDetector::Detect(in))
    {
    case PYLLBC_NONE_OBJ:
        rtn = LLBC_OK;
        out = LLBC_New1(LLBC_JsonValue, Json::nullValue);
        break;

    case PYLLBC_BOOL_OBJ:
        {
            bool val;
            if ((rtn = This::EncodeBool(in, val)) != LLBC_OK)
                break;

            out = LLBC_New1(LLBC_JsonValue, val);
        }
        break;

    case PYLLBC_INT_OBJ:
    case PYLLBC_LONG_OBJ:
        {
            sint64 val;
            if ((rtn = This::EncodeInt(in, val)) != LLBC_OK)
                break;

            if (INT_MIN <= val && val <= INT_MAX)
                out = LLBC_New1(LLBC_JsonValue, static_cast<sint32>(val));
            else
                out = LLBC_New1(LLBC_JsonValue, val);
        }
        break;

    case PYLLBC_FLOAT_OBJ:
        {
            double val;
            if ((rtn = This::EncodeFloat(in, val)) != LLBC_OK)
                break;

            out = LLBC_New1(LLBC_JsonValue, val);
        }
        break;

    case PYLLBC_STR_OBJ:
        {
            std::string val;
            if ((rtn = This::EncodeStr(in, val)) != LLBC_OK)
                break;

            out = LLBC_New1(LLBC_JsonValue, val);
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
        rtn = This::EncodeSeq(in, *(out = LLBC_New1(LLBC_JsonValue, Json::arrayValue)));
        break;

    case PYLLBC_DICT_OBJ:
        rtn = This::EncodeDict(in, *(out = LLBC_New1(LLBC_JsonValue, Json::objectValue)));
        break;

    case PYLLBC_UNKNOWN_OBJ:
        rtn = This::EncodeInst(in, *(out = LLBC_New1(LLBC_JsonValue, Json::objectValue)));
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

int pyllbc_ObjCoder::EncodeSeq(PyObject *in, LLBC_JsonValue &out)
{
    PyObject *fastSeq = PySequence_Fast(in, "could not convert seq to FAST seq");
    if (UNLIKELY(!fastSeq))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    const Py_ssize_t len = PySequence_Fast_GET_SIZE(fastSeq);
    ASSERT(len >= 0 && "python API internal error");

    for (register Py_ssize_t i = 0; i < len; i++)
    {
        PyObject *elemObj = PySequence_Fast_GET_ITEM(fastSeq, i);
        if (UNLIKELY(!elemObj))
        {
            Py_DECREF(fastSeq);
            pyllbc_TransferPyError();

            return LLBC_FAILED;
        }

        LLBC_JsonValue *elemJson = NULL;
        if (This::Encode(elemObj, elemJson) != LLBC_OK)
        {
            Py_DECREF(fastSeq);
            return LLBC_FAILED;
        }

        out.append(*elemJson);

        LLBC_Delete(elemJson);
    }

    Py_DECREF(fastSeq);

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeDict(PyObject *in, LLBC_JsonValue &out)
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

        LLBC_JsonValue *valJson = NULL;
        if (This::Encode(val, valJson) != LLBC_OK)
            return LLBC_FAILED;

        out[strKey] = *valJson;

        LLBC_Delete(valJson);
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeInst(PyObject *in, LLBC_JsonValue &out)
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
    for (Py_ssize_t i = 0; i < len; i++)
    {
        PyObject *slotItem = PySequence_ITEM(slots, i);
        PyObject *slotVal = PyObject_GetAttr(in, slotItem);
        if (slotVal)
        {
            LLBC_JsonValue *slotValJson;
            if (This::Encode(slotVal, slotValJson) != LLBC_OK)
            {
                Py_DECREF(slotVal);
                Py_DECREF(slotItem);

                Py_DECREF(slots);

                return LLBC_FAILED;
            }

            char *itemStr;
            if (UNLIKELY(!PyArg_Parse(slotItem, "s", &itemStr)))
            {
                pyllbc_TransferPyError();

                LLBC_Delete(slotValJson);

                Py_DECREF(slotVal);
                Py_DECREF(slotItem);

                Py_DECREF(slots);

                return LLBC_FAILED;
            }

            out[itemStr] = *slotValJson;

            LLBC_Delete(slotValJson);

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
    switch(in.type())
    {
    case Json::nullValue:
        rtn = This::DecodeNull(in, out);
        break;

    case  Json::booleanValue:
        rtn = This::DecodeBool(in, out);
        break;

    case Json::intValue:
        rtn = This::DecodeInt(in, out);
        break;

    case Json::uintValue:
        rtn = This::DecodeUInt(in, out);
        break;

    case Json::longValue:
        rtn = This::DecodeLong(in, out);
        break;

    case Json::ulongValue:
        rtn = This::DecodeULong(in, out);
        break;

    case Json::realValue:
        rtn = This::DecodeReal(in, out);
        break;

    case Json::stringValue:
        rtn = This::DecodeStr(in, out);
        break;

    case Json::arrayValue:
        rtn = This::DecodeArray(in, out);
        break;

    case Json::objectValue:
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
    const bool boolVal = in.asBool();

    out = boolVal ? Py_True : Py_False;
    Py_INCREF(out);

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeInt(const LLBC_JsonValue &in, PyObject *&out)
{
    if(!(out = Py_BuildValue("i", in.asInt())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeUInt(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("I", in.asUInt())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeLong(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("L", in.asLong())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeULong(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("K", in.asULong())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeReal(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("d", in.asDouble())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeStr(const LLBC_JsonValue &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("s", in.asCString())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeArray(const LLBC_JsonValue &in, PyObject *&out)
{
    out = PyTuple_New(in.size());
    if (UNLIKELY(!out))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    Py_ssize_t tupleIdx = 0;
    for (LLBC_JsonValueCIter it = in.begin();
         it != in.end();
         it++)
    {
        const LLBC_JsonValue &elemJson = *it;

        PyObject *elemObj = NULL;
        if (This::_Decode(elemJson, elemObj) != LLBC_OK)
        {
            Py_DECREF(out);
            out = NULL;

            return LLBC_FAILED;
        }

        if (UNLIKELY(PyTuple_SetItem(out, tupleIdx++, elemObj) != 0))
        {
            pyllbc_TransferPyError();

            Py_DECREF(elemObj);

            Py_DECREF(out);
            out = NULL;

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

    for (LLBC_JsonValueCIter it = in.begin();
         it != in.end();
         it++)
    {
        const LLBC_JsonValue &key = it.key();
        if (UNLIKELY(key.type() != Json::stringValue))
        {
            pyllbc_SetError("dict key not string type", LLBC_ERROR_INVALID);

            Py_DECREF(out);
            out = NULL;

            return LLBC_FAILED;
        }

        PyObject *elemObj = NULL;
        const LLBC_JsonValue &elemJson = *it;
        if (This::_Decode(elemJson, elemObj) != LLBC_OK)
        {
            Py_DECREF(out);
            out = NULL;

            return LLBC_FAILED;
        }

        if (UNLIKELY(PyDict_SetItemString(out, key.asCString(), elemObj) != 0))
        {
            pyllbc_TransferPyError();
            
            Py_DECREF(elemObj);

            Py_DECREF(out);
            out = NULL;

            return LLBC_FAILED;
        }

        Py_DECREF(elemObj);
    }

    return LLBC_OK;
}
