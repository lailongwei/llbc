/**
 * @file    _Lullbc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-15
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Export.h"
#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"

// API: Startup
LULLBC_LUA_METH int _lullbc_Startup(lua_State *l)
{
    bool initWinSock = true;
#if LLBC_TARGET_PLATFORM_WIN32
    if (lua_gettop(l) > 0)
        initWinSock = lua_toboolean(l, 1) != 0;

#endif
    int ret = LLBC_Startup(initWinSock);
    if (ret != LLBC_OK)
        lullbc_TransferLLBCError(
            l, __FILE__, __LINE__, "when startup llbc core library");

    return 0;
}

// API: Cleanup
LULLBC_LUA_METH int _lullbc_Cleanup(lua_State *l)
{
    LLBC_Cleanup();
    return 0;
}