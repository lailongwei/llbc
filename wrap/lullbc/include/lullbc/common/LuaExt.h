/**
 * @file    LuaExt.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-16
 * @version 1.0
 *
 * @brief
 */
#ifndef __LULLBC_COM_LUAEXT_H__
#define __LULLBC_COM_LUAEXT_H__

#include "lullbc/common/LibHeader.h"

// Sume easy-to-use type convert methods define.
#define lua_toint16(l, i)   static_cast<sint16>(lua_tointeger(l, i))
#define lua_touint16(l, i)  static_cast<uint16>(lua_tointeger(l, i))
#define lua_toint32(l, i)   static_cast<sint32>(lua_tointeger(l, i))
#define lua_touint32(l, i)  static_cast<uint32>(lua_tointeger(l, i))
#define lua_toint64(l, i)   lua_tointeger(l, i)
#define lua_touint64(l, i)  static_cast<uint64>(lua_tointeger(l, i))

// lua_objlen check, before 5.2, lua_objlen is supported, but after 5.2, this method changed to lua_rawlen.
#if LUA_VERSION_NUM > 501 && !defined(lua_objlen)
 #define lua_objlen(l, i)    lua_rawlen(l, (i))
#endif

#endif // !__LULLBC_COM_LUAEXT_H__
