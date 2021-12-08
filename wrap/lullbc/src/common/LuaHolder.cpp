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

#include "lullbc/common/Export.h"

#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"

#include "lullbc/common/LuaHolder.h"

void lullbc_LuaHolder::Hold(lua_State *l, int fromStackPos, const char *toKey, bool remove)
{
    if (UNLIKELY(l == nullptr || toKey == nullptr))
        lullbc_SetError(l, LLBC_ERROR_ARG);

    if (UNLIKELY(lua_isnil(l, fromStackPos)))
        lullbc_SetError(l, LLBC_ERROR_ARG);

    PushHoldTable(l);

    lua_pushvalue(l, fromStackPos < 0 ? fromStackPos - 1 : fromStackPos);
    lua_setfield(l, -2, toKey);

    lua_remove(l, -1);

    if (remove)
        lua_remove(l, fromStackPos);
}

void lullbc_LuaHolder::Hold(lua_State *l, const char *fromGlobalKey, const char *toKey, bool remove)
{
    if (UNLIKELY(l == nullptr || fromGlobalKey == nullptr || toKey == nullptr))
        lullbc_SetError(l, LLBC_ERROR_ARG);

    PushHoldTable(l);

    lua_getglobal(l, fromGlobalKey);
    if (UNLIKELY(lua_isnil(l, -1)))
    {
        lua_settop(l, lua_gettop(l) - 2);
        lullbc_SetError(l, LLBC_ERROR_ARG);
    }

    lua_setfield(l, -2, toKey);

    lua_remove(l, -1);

    if (remove)
    {
        lua_pushnil(l);
        lua_setglobal(l, fromGlobalKey);
    }
}

void lullbc_LuaHolder::UnHold(lua_State *l, const char *key)
{
    if (UNLIKELY(l == nullptr || key == nullptr))
        lullbc_SetError(l, LLBC_ERROR_ARG);

    PushHoldTable(l);
    lua_pushnil(l);
    lua_setfield(l, -2, key);

    lua_remove(l, -1);
}

void lullbc_LuaHolder::Get(lua_State *l, const char *key)
{
    if (UNLIKELY(l == nullptr || key == nullptr))
        lullbc_SetError(l, LLBC_ERROR_ARG);

    PushHoldTable(l);
    lua_getfield(l, -1, key);

    lua_remove(l, -2);
}

void lullbc_LuaHolder::PushHoldTable(lua_State *l)
{
    lua_getglobal(l, LULLBC_HOLD_TABLE_KEY);
    if (LIKELY(lua_istable(l, -1)))
        return;

    // Remove not llbc hold_table.
    lua_remove(l, -1);

    // Create new hold_table.
    lua_newtable(l);
    lua_pushvalue(l, -1);
    lua_setglobal(l, LULLBC_HOLD_TABLE_KEY);
}
