/**
 * @file    _Util_Misc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-22
 * @version 1.0
 *
 * @brief
 */

// Api: Util_Misc_ToPointer
LULLBC_LUA_METH int _lullbc_Util_Misc_ToPointer(lua_State *l)
{
    lua_pushinteger(l, reinterpret_cast<lua_Integer>(lua_topointer(l, 1)));
    return 1;
}
