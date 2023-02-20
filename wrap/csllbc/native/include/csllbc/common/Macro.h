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

#include "llbc/common/Macro.h"

// the hidden/export/exporting macro define, same with llbc core library.
#ifndef CSLLBC_HIDDEN
 #define CSLLBC_HIDDEN LLBC_HIDDEN
#endif

#ifndef CSLLBC_EXPORT
 #define CSLLBC_EXPORT LLBC_EXPORT
#endif

#ifndef CSLLBC_EXPORTING
 #define CSLLBC_EXPORTING LLBC_EXPORTING
#endif

// the __stdcall macro define only available in non-windows platform.
#if LLBC_TARGET_PLATFORM_NON_WIN32
 #if LLBC_TARGET_PLATFORM_IPHONE
  #define __stdcall
 #else // Non-Win32 & Non-iPhone
  #define __stdcall __attribute__((__stdcall__))
 #endif // iPhone
#endif // Non-Win32


