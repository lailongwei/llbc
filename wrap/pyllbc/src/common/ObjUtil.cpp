/**
 * @file    ObjUtil.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/12/27
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/Errors.h"
#include "pyllbc/common/ObjUtil.h"

namespace
{
    LLBC_String __g_invalidObjStr;
}

LLBC_String pyllbc_ObjUtil::GetObjStr(PyObject *obj)
{
    PyObject *pyStr = PyObject_Str(obj);
    if (UNLIKELY(!pyStr))
    {
        pyllbc_TransferPyError();
        return __g_invalidObjStr;
    }

    char *strBeg;
    Py_ssize_t strLen;
    if (UNLIKELY(PyString_AsStringAndSize(pyStr, &strBeg, &strLen) == -1))
    {
        pyllbc_TransferPyError("When get python object string representation");
        Py_DECREF(pyStr);

        return __g_invalidObjStr;
    }

    LLBC_String str(strBeg, strLen);
    Py_DECREF(pyStr); //! Must later DECREF.

    return str;
}
