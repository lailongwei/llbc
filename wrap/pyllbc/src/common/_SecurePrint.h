/**
 * @file    _SecurePrint.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/16
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/LibHeader.h"

LLBC_EXTERN_C PyObject *_pyllbc_output(PyObject *self, PyObject *args)
{
    PyObject *obj = NULL;
    int outputDest = 1;
    if (!PyArg_ParseTuple(args, "|Oi", &obj, &outputDest))
        return NULL;

    if (!obj)
        Py_RETURN_NONE;

    PyObject *strObj = obj;
    if (!PyObject_IsInstance(obj, PYLLBC_STR_CLS))
    {
        if (!(strObj = PyObject_Str(obj)))
            return NULL;
    }
    else
    {
        Py_INCREF(strObj);
    }

    char *str;
    if (!PyArg_Parse(strObj, "s", &str))
    {
        Py_DECREF(strObj);
        return NULL;
    }

    LLBC_Print("%s", str);
    Py_DECREF(strObj);

    Py_RETURN_NONE;
}
