/**
 * @file    Errors.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-06
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/Export.h"

#include "lullbc/common/Errors.h"

void lullbc_TransferLLBCError(lua_State *l, const char *additionMsg, ...)
{
    {
        // Format llbc error.
        LLBC_String errMsg;
        errMsg.format("%s", LLBC_FormatLastError());

        // Format addition message.
        if (additionMsg != NULL)
        {
            char *fmttedAdditionMsg = NULL; int fmttedAdditionMsgLen = 0;
            LLBC_FormatArg(additionMsg, fmttedAdditionMsg, fmttedAdditionMsgLen);

            errMsg.format("%s, addition error info: %s", LLBC_FormatLastError(), fmttedAdditionMsg);
            LLBC_Free(fmttedAdditionMsg); // !!!Free formatted addition message.
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
        if (additionMsg != NULL)
        {
            char *fmttedAdditionMsg = NULL; int fmttedAdditionMsgLen = 0;
            LLBC_FormatArg(additionMsg, fmttedAdditionMsg, fmttedAdditionMsgLen);

            errMsg.append_format(", addition error info: %s", fmttedAdditionMsg);
            LLBC_Free(fmttedAdditionMsg); // !!!Free formatted addition message.
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

    if (additionMsg == NULL)
    {
        lullbc_TransferLLBCError(l, NULL);
        return;
    }
 
   {
        // Format llbc error.
        LLBC_String errMsg;
        errMsg.format("%s", LLBC_FormatLastError());

        // Format addition message.
        if (additionMsg != NULL)
        {
            char *fmttedAdditionMsg = NULL; int fmttedAdditionMsgLen = 0;
            LLBC_FormatArg(additionMsg, fmttedAdditionMsg, fmttedAdditionMsgLen);

            errMsg.format("%s, addition error info: %s", LLBC_FormatLastError(), fmttedAdditionMsg);
            LLBC_Free(fmttedAdditionMsg); // !!!Free formatted addition message.
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
    if (extraMsg != NULL)
    {

        char *fmttedExtraMsg = NULL; int fmttedExtraMsgLen = 0;
        LLBC_FormatArg(extraMsg, fmttedExtraMsg, fmttedExtraMsgLen);

        LLBC_String msg;
        msg.append(fmttedExtraMsg, fmttedExtraMsgLen);

        LLBC_Free(fmttedExtraMsg);

        luaL_argcheck(l, cond, arg, msg.c_str());
    }
    else
    {
        luaL_argcheck(l, cond, arg, extraMsg);
    }
}