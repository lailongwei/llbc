/**
 * @file    _GUIDHelper.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-26
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Export.h"
#include "lullbc/common/Macro.h"
#include "lullbc/common/Errors.h"

// Api: GUID_Generate
LULLBC_LUA_METH int _lullbc_GUID_Generate(lua_State *l)
{
    const LLBC_String guid = LLBC_GUIDHelper::GenStr();
    lua_pushlstring(l, guid.data(), guid.size());
    return 1;
}