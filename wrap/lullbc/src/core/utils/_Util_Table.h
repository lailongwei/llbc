/**
 * @file    _Util_Table.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-22
 * @version 1.0
 *
 * @brief
 */

// Api: Util_Table_Concat
LULLBC_LUA_METH int _lullbc_Util_Table_Concat(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TTABLE);
    luaL_checktype(l, 2, LUA_TSTRING);

    size_t fmtStrLen;
    const char *fmtStr = lua_tolstring(l, 2, &fmtStrLen);

    LLBC_String concatResult;
    size_t tableLen = lua_objlen(l, 1);
    for (size_t i = 1; i <= tableLen; i++)
    {
        lua_geti(l, 1, i);

        size_t len;
        const char *str = luaL_tolstring(l, -1, &len);
        if (i > 1)
            concatResult.append(fmtStr, fmtStrLen);
        concatResult.append(str, len);

        lua_pop(l, 2);
    }

    lua_pushlstring(l, concatResult.data(), concatResult.size());

    return 1;
}
