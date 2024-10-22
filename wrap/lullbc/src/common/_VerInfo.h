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

// API: GetVersionInfo
LULLBC_LUA_METH int _lullbc_GetVersionInfo(lua_State *l)
{
    bool verbose = false;
    if (lua_gettop(l) > 0)
        verbose = lua_toboolean(l, 1) != 0;

    LLBC_String verInfo = LLBC_GetVersionInfo(verbose);
    const size_t insertPos = verbose ? verInfo.find('\n') - 1 : verInfo.length() - 1;
    verInfo.insert(insertPos, LLBC_String().format(", lua ver:%s.%s", LUA_VERSION_MAJOR, LUA_VERSION_MINOR));

    lua_pushlstring(l, verInfo.c_str(), verInfo.length());

    return 1;
}
