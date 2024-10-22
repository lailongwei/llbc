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

#pragma once

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


