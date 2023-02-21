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

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

/**
 * Common return values define.
 */
#define LLBC_OK     (0)
#define LLBC_FAILED (-1)

/**
 * carriage return / line feed.
 */
#define LLBC_CR_A         '\r'
#define LLBC_LF_A         '\n'
#define LLBC_CRLF_A       "\r\n"
#define LLBC_CR_W         L##'\r'
#define LLBC_LF_W         L##'\n'
#define LLBC_CRLF_W       L##"\r\n"
#define LLBC_CR           LLBC_TEXT('\r')
#define LLBC_LF           LLBC_TEXT('\n')
#define LLBC_CRLF         LLBC_TEXT("\r\n")

// slash('/') and backlash('\\') define.
#define LLBC_SLASH_A      '/'
#define LLBC_BACKLASH_A   '\\'
#define LLBC_SLASH_W      L##'/'
#define LLBC_BACKLASH_W   L##'\\'
#define LLBC_SLASH        LLBC_TEXT('/')
#define LLBC_BACKLASH     LLBC_TEXT('\\')

// Wait infinite macro define.
#if LLBC_TARGET_PLATFORM_NON_WIN32
#define LLBC_INFINITE    ((int)(0xffffffff))
#else // LLBC_TARGET_PLATFORM_WIN32
#define LLBC_INFINITE    INFINITE
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

__LLBC_NS_END
