/**
 * @file    _Util_String.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-22
 * @version 1.0
 *
 * @brief
 */

// API: Util_String_HashString
LULLBC_LUA_METH int _lullbc_Util_String_HashString(lua_State *l)
{
    if (lua_gettop(l) < 1)
        lullbc_SetError(l, LLBC_ERROR_ARG);

    size_t strLen;
    const char *str = lua_tolstring(l, 1, &strLen);
    if (str == NULL)
    {
        lua_pushinteger(l, 1);
        return 1;
    }

    lua_pushinteger(l, LLBC_HashString(str, strLen));
    return 1;
}