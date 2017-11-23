/**
 * @file    LuaHolder.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-11-20
 * @version
 *
 * @brief   The library data class encapsulation.
 */

#include "lullbc/common/Export.h"

#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"

#include "lullbc/common/LuaHolder.h"

void lullbc_LuaHolder::Hold(lua_State *l, int fromStackPos, const char *toKey, bool remove)
{
    if (UNLIKELY(l == NULL || toKey == NULL))
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
    if (UNLIKELY(l == NULL || fromGlobalKey == NULL || toKey == NULL))
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
    if (UNLIKELY(l == NULL || key == NULL))
        lullbc_SetError(l, LLBC_ERROR_ARG);

    PushHoldTable(l);
    lua_pushnil(l);
    lua_setfield(l, -2, key);

    lua_remove(l, -1);
}

void lullbc_LuaHolder::Get(lua_State *l, const char *key)
{
    if (UNLIKELY(l == NULL || key == NULL))
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
