/**
 * @file    _Codec.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/26
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"
#include "pyllbc/comm/PyObjCoder.h"

LLBC_EXTERN_C PyObject *_pyllbc_encode(PyObject *self, PyObject *args)
{
    PyObject *o;
    if (!PyArg_ParseTuple(args, "O", &o))
        return NULL;

    std::string str;
    if (pyllbc_ObjCoder::Encode(o, str) != LLBC_RTN_OK)
        return NULL;

    return PyString_FromStringAndSize(str.data(), str.size());
}

LLBC_EXTERN_C PyObject *_pyllbc_decode(PyObject *self, PyObject *args)
{
    char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;

    PyObject *o;
    if (pyllbc_ObjCoder::Decode(str, o) != LLBC_RTN_OK)
        return NULL;

    return Py_BuildValue("N", o);
}
