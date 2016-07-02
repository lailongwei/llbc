/**
 * @file    _Util_String.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/07/02
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Common.h"

LLBC_EXTERN_C PyObject *_pyllbc_HashString(PyObject *self, PyObject *args)
{
    char *cstr; int strLen;
    if (!PyArg_ParseTuple(args, "s#", &cstr, &strLen))
        return NULL;

    int hashVal = LLBC_HashString(cstr, strLen);
    return PyInt_FromLong(hashVal);
}