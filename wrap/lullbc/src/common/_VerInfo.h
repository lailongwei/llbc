/**
 * @file    _VerInfo.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-07
 * @version 1.0
 *
 * @brief
 */

// API: GetVersionInfo
LULLBC_LUA_METH int _lullbc_GetVersionInfo(lua_State *l)
{
    bool verbose = false;
    if (lua_gettop(l) > 0)
        verbose = lua_toboolean(l, 1) != 0;

    const LLBC_String verInfo = LLBC_GetVersionInfo(verbose);
    lua_pushlstring(l, verInfo.c_str(), verInfo.length());

    return 1;
}
