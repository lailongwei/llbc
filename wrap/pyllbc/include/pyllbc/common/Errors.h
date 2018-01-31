/**
 * @file    Errors.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/11
 * @version 1.0
 *
 * @brief   pyllbc error about constants and APIs define.
 *          custom errno range: [65530, 65534]
 */
#ifndef __PYLLBC_COM_ERROR_H__
#define __PYLLBC_COM_ERROR_H__

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
 * @param[out] traceback - the error traceback, borrowed reference, maybe NULL.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_PyErrFetch(PyObject *&errType, LLBC_String &errStr, PyObject *&traceback);

/**
 * Transfer llbc library error to python.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_TransferLLBCError(const char *file = NULL, int lineNo = 0, const LLBC_String &additionalMsg = "");

/**
 * Transfer python error to llbc.
  *@param[in] additionalMsg - the additional message.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_TransferPyError(const LLBC_String &additionalMsg = "");

/**
 * Set pyllbc module last error.
 * @param[in] errNo - must according to llbc library errno constraint.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_SetError(int errNo);

/**
 * Set pyllbc module error.
 * @param[in] errDesc   - the error describe.
 * @param[in] llbcErr   - the llbc library error no.
 * @param[in] pyErrType - the python error, normal, not steal reference.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_SetError(const LLBC_String &errDesc, 
                                             int llbcErr = PYLLBC_ERROR_COMMON,
                                             PyObject *pyErrType = NULL);

/**
 * Clear llbc library error and python error.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_ClearError();

/**
 * Error set hook setter.
 * @param[in] hook - the error setter hook.
 *                   Hook arguments describe:
 *                     const LLBC_String & - the error string.
 *                     int                 - llbc library error no.
 *                     PyObject *          - exception class.
 *                     PyObject *          - traceback object.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_SetErrSetHock(LLBC_IDelegate4<void, const LLBC_String &, int, PyObject *, PyObject *> *hook);

/**
 * Error clear hook setter.
 * @param[in] hook - the error clear hook.
 */
LLBC_EXTERN LLBC_HIDDEN void pyllbc_SetErrClearHook(LLBC_IDelegate0<void> *hook);

#endif // !__PYLLBC_COM_ERROR_H__
