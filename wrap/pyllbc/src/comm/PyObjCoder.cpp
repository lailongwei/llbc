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

}

int pyllbc_ObjCoder::Encode(PyObject *in, std::string &out)
{
    std::shared_ptr<LLBC_Json> autoJson(new LLBC_Json());
    LLBC_Json *ptr = autoJson.get();
    if (This::Encode(in, ptr) != LLBC_OK)
    {
        return LLBC_FAILED;
    }

    out = autoJson->dump();

    return LLBC_OK;
}

int pyllbc_ObjCoder::Decode(const std::string &in, PyObject *&out)
{
    LLBC_Json j;
    try
    {
        j = LLBC_Json::parse(in);
        if (!j.is_array())
        {
            pyllbc_SetError("top json type not array", LLBC_ERROR_FORMAT);
            return LLBC_FAILED;
        }
        else if (j.size() < 1)
        {
            pyllbc_SetError("top json size lt 1", LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
    }
    catch (std::exception &e)
    {
        LLBC_String err;
        err.format("could not decode json string, exception err:%s",  e.what());
        pyllbc_SetError(err);
        return LLBC_FAILED;
    }
    catch (...)
    {
        LLBC_String err;
        err.format("unknown exception error, could not decode json string");
        pyllbc_SetError(err);
        return LLBC_FAILED;
    }

    return This::_Decode(j[0u], out);
}

int pyllbc_ObjCoder::Encode(PyObject *in, LLBC_Json *&out)
{
    int rtn = LLBC_FAILED;
    switch (pyllbc_TypeDetector::Detect(in))
    {
    case PYLLBC_NONE_OBJ:
        {
            rtn = LLBC_OK;
            *out = nullptr;
        }
        break;

    case PYLLBC_BOOL_OBJ:
        {
            bool val;
            if ((rtn = This::EncodeBool(in, val)) != LLBC_OK)
                break;

            *out = val;
        }
        break;

    case PYLLBC_INT_OBJ:
    case PYLLBC_LONG_OBJ:
        {
            sint64 val;
            if ((rtn = This::EncodeInt(in, val)) != LLBC_OK)
                break;

            if (INT_MIN <= val && val <= INT_MAX)
            {
                *out = static_cast<sint32>(val);
            }
            else
                *out = val;
        }
        break;

    case PYLLBC_FLOAT_OBJ:
        {
            double val;
            if ((rtn = This::EncodeFloat(in, val)) != LLBC_OK)
                break;

            *out = val;
        }
        break;

    case PYLLBC_STR_OBJ:
        {
            std::string val;
            if ((rtn = This::EncodeStr(in, val)) != LLBC_OK)
                break;

            *out = val;
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
            *out = LLBC_Json::array();
            rtn = This::EncodeSeq(in, *out);
        }
        break;

    case PYLLBC_DICT_OBJ:
        {
            *out = LLBC_Json::object(); 
            rtn = This::EncodeDict(in, *out);
        }
        break;

    case PYLLBC_UNKNOWN_OBJ:
        {
            *out = LLBC_Json::object(); 
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

int pyllbc_ObjCoder::EncodeSeq(PyObject *in, LLBC_Json &out)
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

        std::shared_ptr<LLBC_Json> elemJson(new LLBC_Json());
        LLBC_Json *ptr = elemJson.get();
        if (This::Encode(elemObj, ptr) != LLBC_OK)
        {
            Py_DECREF(fastSeq);
            return LLBC_FAILED;
        }

        out.push_back(*elemJson);
    }

    Py_DECREF(fastSeq);

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeDict(PyObject *in, LLBC_Json &out)
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

        std::shared_ptr<LLBC_Json> valJson(new LLBC_Json());
        LLBC_Json *ptr = valJson.get();
        if (This::Encode(val, ptr) != LLBC_OK)
        {
            return LLBC_FAILED;
        }

        out[strKey] = *valJson;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::EncodeInst(PyObject *in, LLBC_Json &out)
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
            std::shared_ptr<LLBC_Json> slotValJson(new LLBC_Json());
            LLBC_Json *ptr = slotValJson.get();
            if (This::Encode(slotVal, ptr) != LLBC_OK)
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

                Py_DECREF(slotVal);
                Py_DECREF(slotItem);

                Py_DECREF(slots);

                return LLBC_FAILED;
            }

            out[itemStr] = *slotValJson;

            Py_DECREF(slotVal);
        }

        Py_DECREF(slotItem);
    }

    Py_DECREF(slots);

    return LLBC_OK;
}

int pyllbc_ObjCoder::_Decode(const LLBC_Json &in, PyObject *&out)
{
    int rtn = LLBC_OK;
    switch(in.type())
    {
    case LLBC_Json::value_t::null:
        rtn = This::DecodeNull(in, out);
        break;

    case LLBC_Json::value_t::boolean:
        rtn = This::DecodeBool(in, out);
        break;

    case LLBC_Json::value_t::number_float:
    case LLBC_Json::value_t::number_integer:
    case LLBC_Json::value_t::number_unsigned:

        // 浮点数
        if (in.is_number_float())
        {
            rtn = This::DecodeReal(in, out);
        }

        // 整数类型
        else if(in.is_number_integer())
        {
            // 无符号
            if(in.is_number_unsigned())
            {
                constexpr uint64 MAX_32 = static_cast<uint64>(std::numeric_limits<uint32>::max());
                uint64 value = in.get<uint64>();
                if(value > MAX_32)
                {
                    rtn = This::DecodeULong(in, out);
                }
                else
                {
                    rtn = This::DecodeUInt(in, out);
                }
            }

            // 有符号
            else
            {
                constexpr sint64 MAX_32 = static_cast<sint64>(std::numeric_limits<sint32>::max());
                sint64 value = in.get<sint64>();
                if(value > MAX_32)
                {
                    rtn = This::DecodeLong(in, out);
                }
                else
                {
                    rtn = This::DecodeInt(in, out);
                }
            }
        }

        // 其他类型
        else
        {
            rtn = This::DecodeULong(in, out);
        }
        break;

    case LLBC_Json::value_t::string:
        rtn = This::DecodeStr(in, out);
        break;

    case LLBC_Json::value_t::array:
        rtn = This::DecodeArray(in, out);
        break;

    case LLBC_Json::value_t::object:
        rtn = This::DecodeObj(in, out);
        break;

    default:
        pyllbc_SetError(LLBC_String().append_format("not support json object type:%d(%s)", in.type(), in.type_name()), LLBC_ERROR_NOT_IMPL);
        rtn = LLBC_FAILED;
        break;
    }

    return rtn;
}

int pyllbc_ObjCoder::DecodeNull(const LLBC_Json &in, PyObject *&out)
{
    Py_INCREF(Py_None);
    out = Py_None;

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeBool(const LLBC_Json &in, PyObject *&out)
{
    const bool boolVal = in.get<bool>();

    out = boolVal ? Py_True : Py_False;
    Py_INCREF(out);

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeInt(const LLBC_Json &in, PyObject *&out)
{
    if(!(out = Py_BuildValue("i", in.get<sint32>())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeUInt(const LLBC_Json &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("I", in.get<uint32>())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeLong(const LLBC_Json &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("L", in.get<sint64>())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeULong(const LLBC_Json &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("K", in.get<uint64>())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeReal(const LLBC_Json &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("d", in.get<double>())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeStr(const LLBC_Json &in, PyObject *&out)
{
    if (!(out = Py_BuildValue("s", in.get<LLBC_String>())))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int pyllbc_ObjCoder::DecodeArray(const LLBC_Json &in, PyObject *&out)
{
    out = PyTuple_New(in.size());
    if (UNLIKELY(!out))
    {
        pyllbc_TransferPyError();
        return LLBC_FAILED;
    }

    Py_ssize_t tupleIdx = 0;
    for (auto it = in.begin();
         it != in.end();
         ++it)
    {
        const LLBC_Json &elemJson = *it;

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

int pyllbc_ObjCoder::DecodeObj(const LLBC_Json &in, PyObject *&out)
{
    if (UNLIKELY(!(out = PyDict_New())))
    {
        LLBC_SetLastError(PYLLBC_ERROR_COMMON);
        return LLBC_FAILED;
    }

    for (auto it = in.begin();
         it != in.end();
         ++it)
    {
        const LLBC_Json &key = it.key();
        if (UNLIKELY(key.type() != LLBC_Json::value_t::string))
        {
            pyllbc_SetError(LLBC_String().append_format("dict key not string type:%d(%s)", key.type(), key.type_name()), LLBC_ERROR_INVALID);

            Py_DECREF(out);
            out = nullptr;

            return LLBC_FAILED;
        }

        PyObject *elemObj = nullptr;
        const LLBC_Json &elemJson = it.value();
        if (This::_Decode(elemJson, elemObj) != LLBC_OK)
        {
            Py_DECREF(out);
            out = nullptr;

            return LLBC_FAILED;
        }

        if (UNLIKELY(PyDict_SetItemString(out, it.key().c_str(), elemObj) != 0))
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
