/**
 * @file    lullbc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017/07/14
 * @version 1.0
 *
 * @brief
 */

#include "lullbc/common/Export.h"

#include "lullbc.h"
#include "_LullbcMethods.h"

int lullbc_entry(lua_State *l)
{
    luaL_checkversion(l);
    luaL_newlib(l, lullbc_NativeMethods);

    return 1;
}