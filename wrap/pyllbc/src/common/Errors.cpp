// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "pyllbc/common/Export.h"

#include "pyllbc/common/ObjUtil.h"
#include "pyllbc/common/Errors.h"

PyObject *pyllbc_Exception = nullptr;

LLBC_Delegate<void()> pyllbc_ErrClearHook;
LLBC_Delegate<void(const LLBC_String &, int, PyObject *, PyObject *)> pyllbc_ErrSetHook;

void pyllbc_PyErrFetch(PyObject *&errType, LLBC_String &errStr, PyObject *&traceback)
{
    if (!PyErr_Occurred())
    {
        errType = nullptr;
        errStr.clear();
        traceback = nullptr;

        return;
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
        PyObject *tbObj = nullptr;
        pyllbc_PyErrFetch(errCls, errStr, tbObj);
        pyllbc_ErrSetHook(errDesc, LLBC_GetLastError(), errCls, tbObj);
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

            pyllbc_ErrSetHook(errStr, LLBC_GetLastError(), errType, tbObj);
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
            PyObject *tbObj = nullptr;
            pyllbc_PyErrFetch(pyErrType, errStr, tbObj);

            pyllbc_ErrSetHook(errStr, llbcErr, pyErrType, tbObj);
        }
    }

    LLBC_SetLastError(llbcErr);
}

void pyllbc_ClearError()
{
    PyErr_Clear();
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);

    if (pyllbc_ErrClearHook)
        pyllbc_ErrClearHook();
}

void pyllbc_SetErrSetHock(const LLBC_Delegate<void(const LLBC_String &, int, PyObject *, PyObject *)> &hook)
{
    pyllbc_ErrSetHook = hook;
}

void pyllbc_SetErrClearHook(const LLBC_Delegate<void()> &hook)
{
    pyllbc_ErrClearHook = hook;
}
