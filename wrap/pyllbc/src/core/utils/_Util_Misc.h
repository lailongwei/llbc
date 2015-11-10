/**
 * @file    _Util_Misc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/06
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Common.h"

LLBC_EXTERN_C PyObject *_pyllbc_refcnt(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;

#if LLBC_TARGET_PLATFORM_WIN32 && defined(_WIN64)
    return PyInt_FromLong(static_cast<long>(obj->ob_refcnt - 1));
#else
    return PyInt_FromLong(obj->ob_refcnt - 1);
#endif // target platform is WIN32 and defined _WIN64 macro.
}
