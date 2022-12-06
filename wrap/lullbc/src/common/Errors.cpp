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

#include "lullbc/common/Export.h"

#include "lullbc/common/Errors.h"

void lullbc_TransferLLBCError(lua_State *l, const char *additionMsg, ...)
{
    {
        // Format llbc error.
        LLBC_String errMsg;
        errMsg.format("%s", LLBC_FormatLastError());

        // Format addition message.
        if (additionMsg != nullptr)
        {
            char *fmttedAdditionMsg = nullptr; int fmttedAdditionMsgLen = 0;
            LLBC_FormatArg(additionMsg, fmttedAdditionMsg, fmttedAdditionMsgLen);

            errMsg.format("%s, addition error info: %s", LLBC_FormatLastError(), fmttedAdditionMsg);
            free(fmttedAdditionMsg); // !!!Free formatted addition message.
        }

        // Push to stack top.
        lua_pushlstring(l, errMsg.data(), errMsg.size());
    }

    // Raise lua error.
    lua_error(l);
}

void lullbc_TransferLLBCError(lua_State *l, const char *file, int lineNo, const char *additionMsg, ...)
{
    {
        // Format llbc error, fileline info.
        LLBC_String errMsg;
        errMsg.format("%s", LLBC_FormatLastError());
        if (file)
            errMsg.append_format(", raised from c/c++ source %s:%d", file, lineNo);

        // Format addition message, if specific.
        if (additionMsg != nullptr)
        {
            char *fmttedAdditionMsg = nullptr; int fmttedAdditionMsgLen = 0;
            LLBC_FormatArg(additionMsg, fmttedAdditionMsg, fmttedAdditionMsgLen);

            errMsg.append_format(", addition error info: %s", fmttedAdditionMsg);
            free(fmttedAdditionMsg); // !!!Free formatted addition message.
        }
        
        // Push to stack top.
        lua_pushlstring(l, errMsg.data(), errMsg.size());
    }

    // Raise lua error.
    lua_error(l);
}

void lullbc_SetError(lua_State *l, int errNo)
{
    LLBC_SetLastError(errNo);
    lullbc_TransferLLBCError(l);
}

void lullbc_SetError(lua_State *l, const char *additionMsg, ...)
{
    LLBC_SetLastError(LULLBC_ERROR_COMMON);

    if (additionMsg == nullptr)
    {
        lullbc_TransferLLBCError(l, nullptr);
        return;
    }
 
   {
        // Format llbc error.
        LLBC_String errMsg;
        errMsg.format("%s", LLBC_FormatLastError());

        // Format addition message.
        if (additionMsg != nullptr)
        {
            char *fmttedAdditionMsg = nullptr; int fmttedAdditionMsgLen = 0;
            LLBC_FormatArg(additionMsg, fmttedAdditionMsg, fmttedAdditionMsgLen);

            errMsg.format("%s, addition error info: %s", LLBC_FormatLastError(), fmttedAdditionMsg);
            free(fmttedAdditionMsg); // !!!Free formatted addition message.
        }

        // Push to stack top.
        lua_pushlstring(l, errMsg.data(), errMsg.size());
    }

    // Raise lua error.
    lua_error(l);
}

void lullbc_ArgCheck(lua_State *l, int cond, int arg, const char *extraMsg, ...)
{
    if (cond)
        return;

    LLBC_SetLastError(LULLBC_ERROR_COMMON);
    if (extraMsg != nullptr)
    {

        char *fmttedExtraMsg = nullptr; int fmttedExtraMsgLen = 0;
        LLBC_FormatArg(extraMsg, fmttedExtraMsg, fmttedExtraMsgLen);

        LLBC_String msg;
        msg.append(fmttedExtraMsg, fmttedExtraMsgLen);

        free(fmttedExtraMsg);

        luaL_argcheck(l, cond, arg, msg.c_str());
    }
    else
    {
        luaL_argcheck(l, cond, arg, extraMsg);
    }
}