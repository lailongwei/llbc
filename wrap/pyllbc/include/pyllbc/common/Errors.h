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

#pragma once

#include "pyllbc/common/LibHeader.h"

// pyllbc library all errnos define.

// Error No: PYLLBC_ERROR_COMMON
//
// ErrorText:
//
// pyllbc library error.
//
#define PYLLBC_ERROR_COMMON     (LLBC_BuildCustomErrno(0xfffe))


// The pyllbc python exception class define.
LLBC_EXTERN LLBC_HIDDEN PyObject *pyllbc_Exception;


// Some error operator functions define.
/**
 * Fetch python layer error informations.
 * @param[out] errType   - the error class type, borrowed reference.
 * @param[out] errStr    - error string.
 * @param[out] traceback - the error traceback, borrowed reference, maybe nullptr.
 */
LLBC_HIDDEN void pyllbc_PyErrFetch(PyObject *&errType, LLBC_String &errStr, PyObject *&traceback);

/**
 * Transfer llbc library error to python.
 */
LLBC_HIDDEN void pyllbc_TransferLLBCError(const char *file = nullptr, int lineNo = 0, const LLBC_String &additionalMsg = "");

/**
 * Transfer python error to llbc.
  *@param[in] additionalMsg - the additional message.
 */
LLBC_HIDDEN void pyllbc_TransferPyError(const LLBC_String &additionalMsg = "");

/**
 * Set pyllbc module last error.
 * @param[in] errNo - must according to llbc library errno constraint.
 */
LLBC_HIDDEN void pyllbc_SetError(int errNo);

/**
 * Set pyllbc module error.
 * @param[in] errDesc   - the error describe.
 * @param[in] llbcErr   - the llbc library error no.
 * @param[in] pyErrType - the python error, normal, not steal reference.
 */
LLBC_HIDDEN void pyllbc_SetError(const LLBC_String &errDesc,
                                 int llbcErr = PYLLBC_ERROR_COMMON,
                                 PyObject *pyErrType = nullptr);

/**
 * Clear llbc library error and python error.
 */
LLBC_HIDDEN void pyllbc_ClearError();

/**
 * Error set hook setter.
 * @param[in] hook - the error setter hook.
 *                   Hook arguments describe:
 *                     const LLBC_String & - the error string.
 *                     int                 - llbc library error no.
 *                     PyObject *          - exception class.
 *                     PyObject *          - traceback object.
 */
LLBC_HIDDEN void pyllbc_SetErrSetHock(const LLBC_Delegate<void(const LLBC_String &, int, PyObject *, PyObject *)> &hook);

/**
 * Error clear hook setter.
 * @param[in] hook - the error clear hook.
 */
LLBC_HIDDEN void pyllbc_SetErrClearHook(const LLBC_Delegate<void()> &hook);


