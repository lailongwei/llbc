/**
 * @file    _VerInfo.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/09
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/LibHeader.h"

LLBC_EXTERN_C PyObject *_pyllbc_GetVersionInfo(PyObject *obj, PyObject *args)
{
    int verbose;
    if (!PyArg_ParseTuple(args, "i", &verbose))
        return NULL;

    const LLBC_String verInfo = 
        LLBC_GetVersionInfo(verbose ? true : false);

    return PyString_FromString(verInfo.c_str());
}
