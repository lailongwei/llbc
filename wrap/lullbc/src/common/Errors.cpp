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

void lullbc_TransferLLBCError(lua_State *l, const LLBC_String &additionMsg)
{
    lullbc_TransferLLBCError(l, NULL, 0, additionMsg);
}

void lullbc_TransferLLBCError(lua_State *l, const char *file, int lineNo, const LLBC_String &additionMsg)
{
    LLBC_String errDesc;
    errDesc.format("%s", LLBC_FormatLastError());
    if (file)
        errDesc.append_format(", raised from c/c++ source %s:%d", file, lineNo);
    if (!additionMsg.empty())
        errDesc.append_format(", addition info: %s", additionMsg.c_str());

    luaL_error(l, "%s", errDesc.c_str());
}

void lullbc_SetError(lua_State *l, int errNo)
{
    LLBC_SetLastError(errNo);
    lullbc_TransferLLBCError(l);
}

void lullbc_SetError(lua_State *l, const LLBC_String &errDesc)
{
    LLBC_SetLastError(LULLBC_ERROR_COMMON);
    lullbc_TransferLLBCError(l, errDesc);
}