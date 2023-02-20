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

// Define debug macro, same as LLBC_DEBUG
#ifdef LLBC_DEBUG
 #define LULLBC_DEBUG
#endif

// Define LULLBC_EXTERN, LULLBC_EXTERN_C macros to constraint methods/classes.
#define LULLBC_EXTERN LLBC_EXTERN
#define LULLBC_EXTERN_C LLBC_EXTERN_C

// Define Lua wrap methods constraint macro.
#define LULLBC_LUA_METH static

// Define lullbc library entry method name.
#ifdef LULLBC_DEBUG
 #define lullbc_entry luaopen__lullbc_debug
#else
 #define lullbc_entry luaopen__lullbc
#endif

// Define lullbc library table.
#ifdef LULLBC_DEBUG
 #define LULLBC_HOLD_TABLE_KEY "____llbc.holdtable.debug____"
#else
 #define LULLBC_HOLD_TABLE_KEY "____llbc.holdtable____"
#endif

// All lua hold keys define.
#define LULLBC_HOLD_KEY__TIMER_SCHEDULER        "timer_scheduler"
#define LULLBC_HOLD_KEY__TIMER_ERROR_HANDLER    "timer_error_handler"


