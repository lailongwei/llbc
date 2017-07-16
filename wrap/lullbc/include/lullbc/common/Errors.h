/**
 * @file    Errors.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-06
 * @version 1.0
 *
 * @brief
 */
#ifndef __LULLBC_COM_ERRORS_H__
#define __LULLBC_COM_ERRORS_H__

#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Export.h"


// lua library all errnos define.

// Error No: LULLBC_ERROR_COMMON
//
// ErrorText:
//
// lullbc library error.
//
#define LULLBC_ERROR_COMMON     (LLBC_BuildCustomErrno(0xfffd))

/**
 * Transfer llbc core library error to lua layer.
 * @param[in] file        - error occurred code file name.
 * @param[in] lineNo      - error occurred code file line.
 * @param[in] additionMsg - addition message to transfer.
 */
LULLBC_HIDDEN void lullbc_TransferLLBCError(lua_State *l, const LLBC_String &additionMsg = "");
LULLBC_HIDDEN void lullbc_TransferLLBCError(lua_State *l, const char *file, int lineNo, const LLBC_String &additionMsg = "");

/**
 * Set error by errno/errDesc.
 * @param[in] errNo/errDesc - the errno/error describe.
 */
LULLBC_HIDDEN void lullbc_SetError(lua_State *l, int errNo);
LULLBC_HIDDEN void lullbc_SetError(lua_State *l, const LLBC_String &errDesc);

#endif // !__LULLBC_COM_ERRORS_H__
