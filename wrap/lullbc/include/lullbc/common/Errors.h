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
 * @param[in] ...         - addition message format arguments.
 */
LULLBC_HIDDEN void lullbc_TransferLLBCError(lua_State *l, const char *additionMsg = nullptr, ...);
LULLBC_HIDDEN void lullbc_TransferLLBCError(lua_State *l, const char *file, int lineNo, const char *additionMsg = nullptr, ...);

/**
 * Set error by errno/errDesc.
 * @param[in] errNo/errDesc - the errno/error describe.
 */
LULLBC_HIDDEN void lullbc_SetError(lua_State *l, int errNo);
LULLBC_HIDDEN void lullbc_SetError(lua_State *l, const char *errDesc, ...);

/**
 * Check argument, like luaL_argcheck() method.
 */
LULLBC_HIDDEN void lullbc_ArgCheck(lua_State *l, int cond, int arg, const char *extraMsg, ...);


