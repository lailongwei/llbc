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

// Api: Threading_Sleep
LULLBC_LUA_METH int _lullbc_Threading_Sleep(lua_State *l)
{
    uint64 milliSecs = 0;
    if (lua_gettop(l) > 0)
    {
        lullbc_ArgCheck(l, lua_isnumber(l, 1), 1, "sleep arguments must be a number");
        milliSecs = lua_tointeger(l, 1);
    }

    LLBC_Sleep(static_cast<int>(milliSecs));

    return 0;
}

// Api: Threading_GetCurrentProcessId
LULLBC_LUA_METH int _lullbc_Threading_GetCurrentProcessId(lua_State *l)
{
    lua_pushinteger(l, LLBC_GetCurrentProcessId());
    return 1;
}

// Api: Threading_GetCurrentThreadId
LULLBC_LUA_METH int _lullbc_Threading_GetCurrentThreadId(lua_State *l)
{
    lua_pushinteger(l, LLBC_GetCurrentThreadId());
    return 1;
}
