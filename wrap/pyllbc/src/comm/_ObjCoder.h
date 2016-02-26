/**
 * @file    _ObjCoder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/10/13
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/comm/PyObjCoder.h"

LLBC_EXTERN_C PyObject *_pyllbc_o2j(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;


    std::string j;
    if (pyllbc_ObjCoder::Encode(obj, j) != LLBC_OK)
        return NULL;

    return PyString_FromStringAndSize(j.c_str(), j.length());
}

LLBC_EXTERN_C PyObject *_pyllbc_j2o(PyObject *self, PyObject *args)
{
    char *jStr;
    int strLen;
    if (!PyArg_ParseTuple(args, "s#", &jStr, &strLen))
        return NULL;

    if (strLen == 0)
    {
        pyllbc_SetError("no data to be decode");
        return NULL;
    }

    PyObject *o;
    if (pyllbc_ObjCoder::Decode(std::string(jStr, strLen), o) != LLBC_OK)
        return NULL;

    return o;
}
