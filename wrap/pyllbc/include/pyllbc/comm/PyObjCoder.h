/**
 * @file    PyObjCoder.h
 * @author	Longwei Lai<lailongwei@126.com>
 * @date    2014/08/23
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COMM_PYOBJ_CODER_H__
#define __PYLLBC_COMM_PYOBJ_CODER_H__

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
    static int Encode(PyObject *in, LLBC_JsonValue *&out);
    static int EncodeBool(PyObject *in, bool &out);
    static int EncodeInt(PyObject *in, sint64 &out);
    static int EncodeLong(PyObject *in, sint64 &out);
    static int EncodeFloat(PyObject *in, double &out);
    static int EncodeStr(PyObject *in, std::string &out);
    static int EncodeSeq(PyObject *in, LLBC_JsonValue &out);
    static int EncodeDict(PyObject *in, LLBC_JsonValue &out);
    static int EncodeInst(PyObject *in, LLBC_JsonValue &out);

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

#endif // !__PYLLBC_COMM_PYOBJ_CODER_H__
