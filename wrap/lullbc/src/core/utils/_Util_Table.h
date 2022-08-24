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

// Api: Util_Table_Concat
LULLBC_LUA_METH int _lullbc_Util_Table_Concat(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TTABLE);
    luaL_checktype(l, 2, LUA_TSTRING);

    size_t fmtStrLen;
    const char *fmtStr = lua_tolstring(l, 2, &fmtStrLen);

    LLBC_String concatResult;
    const lua_Unsigned tableLen = lua_objlen(l, 1);
    for (lua_Unsigned i = 1; i <= tableLen; ++i)
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
