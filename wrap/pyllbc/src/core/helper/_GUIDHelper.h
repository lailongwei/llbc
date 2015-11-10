/**
 * @file    _GUIDHelper.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/12
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Common.h"

LLBC_EXTERN_C PyObject *_pyllbc_GenGUID(PyObject *self, PyObject *args)
{
    LLBC_String guid = LLBC_GUIDHelper::GenStr();
    return PyString_FromString(guid.c_str());
}
