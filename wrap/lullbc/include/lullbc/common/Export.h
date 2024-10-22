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

// Define LULLBC_EXPORT, LULLBC_HIDDEN macros to constraint methods/classes.
// WIN32
#if LLBC_TARGET_PLATFORM_WIN32
 #ifndef LULLBC_EXPORT
  #define LULLBC_EXPORTING 1
  #define LULLBC_EXPORT __declspec(dllexport)
 #endif
 #ifndef LULLBC_HIDDEN
  #define LULLBC_HIDDEN
 #endif
#else // Non-Win32
 #ifndef LULLBC_EXPORT
  #define LULLBC_EXPORTING 1
  #define LULLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef LULLBC_HIDDEN
  #define LULLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_WIN32


