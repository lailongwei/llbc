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

// Api: Dir_Exists
LULLBC_LUA_METH int _lullbc_Dir_Exists(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *path = lua_tostring(l, 1);

    lua_pushboolean(l, LLBC_File::Exists(path));
    return 1;
}

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

// Api: Dir_IsAbsPath
LULLBC_LUA_METH int _lullbc_Dir_IsAbsPath(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *dir = lua_tostring(l, 1);

    lua_pushboolean(l, LLBC_Directory::IsAbsPath(dir));

    return 1;
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
    for (int i = 1; i <= paramsCount; ++i)
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

// Api: Dir_SplitExt
LULLBC_LUA_METH int _lullbc_Dir_SplitExt(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *path = lua_tostring(l, 1);

    const LLBC_Strings splited = LLBC_Directory::SplitExt(path);
    lua_pushlstring(l, splited[0].c_str(), splited[0].size());
    lua_pushlstring(l, splited[1].c_str(), splited[1].size());

    return 2;
}

// Api: Dir_GetFiles
LULLBC_LUA_METH int _lullbc_Dir_GetFiles(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *path = lua_tostring(l, 1);

    bool recursive = false;
    if (lua_gettop(l) >= 2)
    {
        luaL_checktype(l, 2, LUA_TBOOLEAN);
        recursive = lua_toboolean(l, 2);
    }

    LLBC_Strings files;
    if (LLBC_Directory::GetFiles(path, files, recursive) != LLBC_OK)
    {
        lullbc_TransferLLBCError(l,
            "Get files failed, path:%s, recursive:%s", path, recursive ? "true" : "false");
        return 0;
    }

    lua_newtable(l);
    for (size_t i = 0; i < files.size(); ++i)
    {
        lua_pushinteger(l, i + 1);
        lua_pushlstring(l, files[i].c_str(), files[i].size());
        lua_settable(l, -3);
    }

    return 1;
}

// Api: Dir_GetDirectories
LULLBC_LUA_METH int _lullbc_Dir_GetDirectories(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *path = lua_tostring(l, 1);

    bool recursive = false;
    if (lua_gettop(l) >= 2)
    {
        luaL_checktype(l, 2, LUA_TBOOLEAN);
        recursive = lua_toboolean(l, 2);
    }

    LLBC_Strings directories;
    if (LLBC_Directory::GetDirectories(path, directories, recursive) != LLBC_OK)
    {
        lullbc_TransferLLBCError(l,
            "Get directories failed, path:%s, recursive:%s", path, recursive ? "true" : "false");
        return 0;
    }

    lua_newtable(l);
    for (size_t i = 0; i < directories.size(); ++i)
    {
        lua_pushinteger(l, i + 1);
        lua_pushlstring(l, directories[i].c_str(), directories[i].size());
        lua_settable(l, -3);
    }

    return 1;
}

// Api: Dir_BaseName
LULLBC_LUA_METH int _lullbc_Dir_BaseName(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *path = lua_tostring(l, 1);

    const LLBC_String &baseName = LLBC_Directory::BaseName(path);
    lua_pushlstring(l, baseName.c_str(), baseName.size());

    return 1;
}

// Api: Dir_DirName
LULLBC_LUA_METH int _lullbc_Dir_DirName(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *path = lua_tostring(l, 1);

    const LLBC_String &dirName = LLBC_Directory::DirName(path);
    lua_pushlstring(l, dirName.c_str(), dirName.size());

    return 1;
}

// Api: Dir_CurDir
LULLBC_LUA_METH int _lullbc_Dir_CurDir(lua_State *l)
{
    const LLBC_String &curDir = LLBC_Directory::CurDir();
    lua_pushlstring(l, curDir.c_str(), curDir.size());

    return 1;
}

// Api: Dir_SetCurDir
LULLBC_LUA_METH int _lullbc_Dir_SetCurDir(lua_State *l)
{
    luaL_checktype(l, 1, LUA_TSTRING);
    const char *curDir = lua_tostring(l, 1);

    if (LLBC_Directory::SetCurDir(curDir) != LLBC_OK)
    {
        lullbc_TransferLLBCError(l, "Set cur dir to %s failed", curDir);
        return 0;
    }

    return 0;
}