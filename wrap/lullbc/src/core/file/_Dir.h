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


#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Export.h"
#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"

// Api: Dir_IsDir
LULLBC_LUA_METH int _lullbc_Dir_IsDir(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *dir = lua_tostring(l, 1);

    lua_pushboolean(l, LLBC_Directory::Exists(dir));
    return 1;
}

// Api: Dir_Create
LULLBC_LUA_METH int _lullbc_Dir_Create(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *dir = lua_tostring(l, 1);

    if (LLBC_Directory::Create(dir) != LLBC_OK)
        lullbc_TransferLLBCError(l, "When create directory [%s]", dir);

    return 0;
}

// Api: Dir_Remove
LULLBC_LUA_METH int _lullbc_Dir_Remove(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *dir = lua_tostring(l, 1);

    if (LLBC_Directory::Remove(dir) != LLBC_OK)
        lullbc_TransferLLBCError(l, "When remove directory [%s]", dir);

    return 0;
}

// Api: Dir_AbsPath
LULLBC_LUA_METH int _lullbc_Dir_AbsPath(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *path = lua_tostring(l, 1);

    const LLBC_String absPath = LLBC_Directory::AbsPath(path);
    lua_pushlstring(l, absPath.data(), absPath.size());

    return 1;
}

// Api: Dir_Join
LULLBC_LUA_METH int _lullbc_Dir_Join(lua_State *l)
{
    int paramsCount = lua_gettop(l);
    if (paramsCount == 0)
        lullbc_SetError(l, "Failed to join paths, expect more than 1 parameters to join");

    LLBC_String joined;
    for (int i = 0; i < paramsCount; i++)
    {
        luaL_checktype(l, 1, LUA_TSTRING);

        size_t len;
        const char *pathPart = lua_tolstring(l, i, &len);
        if (len == 0)
            lullbc_SetError(l, "Failed to join paths, all path parts could not be empty string");

        if (joined.empty())
            joined = pathPart;
        else
            joined = LLBC_Directory::Join(joined, pathPart);
    }

    lua_pushlstring(l, joined.data(), joined.size());

    return 1;
}

// Api: Dir_BaseName
// Api: Dir_DirName
// Api: Dir_SplitExt
// Api: Dir_GetFiles
// Api: Dir_GetDirectories

// Api: Dir_ModuleFileName
// Api: Dir_ModuleFileDir
// Api: Dir_GetCurrentDir
// Api: Dir_SetCurrentDir
// Api: Dir_DocDir
// Api: Dir_HomeDir
// Api: Dir_TempDir
// Api: Dir_CacheDir