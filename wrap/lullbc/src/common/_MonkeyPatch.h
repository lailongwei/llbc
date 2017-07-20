/**
 * @file    _MonkeyPatch.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017/07/16
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Export.h"
#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"
#include "lullbc/common/MonkeyPatchType.h"

static int _lullbc_MonkeyPatch_Table(lua_State *l);
static int _lullbc_MonkeyPatchImpl_Table_Concat(lua_State *l);

// Api: MonkeyPatch
// Brief: Perform monkey patch.
LULLBC_LUA_METH int _lullbc_MonkeyPatch(lua_State *l)
{
    int patchType = -1;
    if (lua_gettop(l) >= 1)
        patchType = lua_toint32(l, 1);

    if (patchType == -1)
    {
        _lullbc_MonkeyPatch_Table(l);
    }
    else
    {
        if (patchType == lullbc_MonkeyPatchType::Table)
            _lullbc_MonkeyPatch_Table(l);
        else
            lullbc_SetError(l, "Unknown monkey patch type %d", patchType);
    }

    return 0;
}

// Api(Internal): Patch table.
static int _lullbc_MonkeyPatch_Table(lua_State *l)
{
    int gblType = lua_getglobal(l, "table");
    if (gblType != LUA_TTABLE)
        lullbc_SetError(l, "'table' type[<%s>] not <table>", lua_typename(l, gblType));

    // Patch table.concat.
    lua_pushcfunction(l, _lullbc_MonkeyPatchImpl_Table_Concat);
    lua_setfield(l, -2, "concat");

    lua_pop(l, 1);

    return 0;
}

// Api(Internal): Patch table.concat implement.
static int _lullbc_MonkeyPatchImpl_Table_Concat(lua_State *l)
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
        if (i > 0)
            concatResult.append(fmtStr, fmtStrLen);
        concatResult.append(str, len);

        lua_pop(l, 2);
    }

    lua_pushlstring(l, concatResult.data(), concatResult.size());

    return 1;
}
