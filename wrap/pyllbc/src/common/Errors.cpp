/**
 * @file    Errors.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/11
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/common/ObjUtil.h"
#include "pyllbc/common/Errors.h"

PyObject *pyllbc_Exception = NULL;

LLBC_IDelegate0<void> *pyllbc_ErrClearHook = NULL;
LLBC_IDelegate4<void, const LLBC_String &, int, PyObject *, PyObject *> *pyllbc_ErrSetHook = NULL;

void pyllbc_PyErrFetch(PyObject *&errType, LLBC_String &errStr, PyObject *&traceback)
{
    if (!PyErr_Occurred())
    {
        errType = NULL;
        errStr.clear();
        traceback = NULL;
    }

    PyObject *errVal;
    PyErr_Fetch(&errType, &errVal, &traceback);
    PyErr_NormalizeException(&errType, &errVal, &traceback);

    // Get error string.
    errStr = pyllbc_ObjUtil::GetObjStr(errVal);

    // Collect all traceback info.
    if (traceback)
    {
        PyTracebackObject *tbs = reinterpret_cast<PyTracebackObject *>(traceback);
        do
        {
            PyFrameObject *frameObj = tbs->tb_frame;
            PyCodeObject *codeObj = frameObj->f_code;
            const char *fileName = PyString_AsString(codeObj->co_filename);
            const char *definedName = PyString_AsString(codeObj->co_name);

            errStr.append_format("%c<llbc holded>File: %s, line: %d, in %s",
                                 LLBC_LF,
                                 fileName,
                                 tbs->tb_lineno,
                                 definedName);
        } while ((tbs = tbs->tb_next));
    }

    PyErr_Restore(errType, errVal, traceback);
}

void pyllbc_TransferLLBCError(const char *file, int lineNo, const LLBC_String &additionalMsg)
{
    LLBC_String errDesc;
    errDesc.format("%s", LLBC_FormatLastError());
    if (file)
        errDesc.append_format(", raised from c/c++ source %s:%d", file, lineNo);
    if (!additionalMsg.empty())
        errDesc.append_format(", additional info: %s", additionalMsg.c_str());

    PyObject *errCls = pyllbc_Exception ? pyllbc_Exception : PyExc_Exception;
    if (!PyErr_Occurred())
        PyErr_SetString(errCls, errDesc.c_str());
    if (pyllbc_ErrSetHook)
    {
        LLBC_String errStr;
        PyObject *tbObj = NULL;
        pyllbc_PyErrFetch(errCls, errStr, tbObj);
        pyllbc_ErrSetHook->Invoke(errDesc, LLBC_GetLastError(), errCls, tbObj);
    }
}

void pyllbc_TransferPyError(const LLBC_String &additionalMsg)
{
    if (PyErr_Occurred())
    {
        LLBC_SetLastError(PYLLBC_ERROR_COMMON);
        if (pyllbc_ErrSetHook)
        {
            LLBC_String errStr;
            PyObject *errType, *tbObj;
            pyllbc_PyErrFetch(errType, errStr, tbObj);

            if (!additionalMsg.empty())
                errStr.append_format("%c%s", LLBC_LF, additionalMsg.c_str());

            pyllbc_ErrSetHook->Invoke(errStr, LLBC_GetLastError(), errType, tbObj);
        }
    }
}

void pyllbc_SetError(int errNo)
{
    LLBC_SetLastError(errNo);
    pyllbc_TransferLLBCError();
}

void pyllbc_SetError(const LLBC_String &errDesc, int llbcErr, PyObject *pyErrType)
{
    if (!PyErr_Occurred())
    {
        if (!pyErrType)
            pyErrType = (pyllbc_Exception ? pyllbc_Exception : PyExc_Exception);

        PyErr_SetString(pyErrType, errDesc.c_str());
        if (pyllbc_ErrSetHook)
        {
            LLBC_String errStr;
            PyObject *tbObj = NULL;
            pyllbc_PyErrFetch(pyErrType, errStr, tbObj);

            pyllbc_ErrSetHook->Invoke(errStr, llbcErr, pyErrType, tbObj);
        }
    }

    LLBC_SetLastError(llbcErr);
}

void pyllbc_ClearError()
{
    PyErr_Clear();
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    if (pyllbc_ErrClearHook)
        pyllbc_ErrClearHook->Invoke();
}

void pyllbc_SetErrSetHock(LLBC_IDelegate4<void, const LLBC_String &, int, PyObject *, PyObject *> *hook)
{
    LLBC_XDelete(pyllbc_ErrSetHook);
    pyllbc_ErrSetHook = hook;
}

void pyllbc_SetErrClearHook(LLBC_IDelegate0<void> *hook)
{
    LLBC_XDelete(pyllbc_ErrClearHook);
    pyllbc_ErrClearHook = hook;
}
