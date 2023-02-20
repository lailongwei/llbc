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

// Define LLBC_EXPORT to __declspec(dllexport), win32 platform specific.

// LINUX
#if LLBC_TARGET_PLATFORM_LINUX
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 1
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_LINUX

// WIN32
#if LLBC_TARGET_PLATFORM_WIN32
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 1
  #define LLBC_EXPORT __declspec(dllexport)
 #endif
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN
 #endif
#endif // LLBC_TARGET_PLATFORM_WIN32

// IPHONE
#if LLBC_TARGET_PLATFORM_IPHONE
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 1
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_IPHONE

// MAC
#if LLBC_TARGET_PLATFORM_MAC
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 1
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_MAC

// ANDROID
#if LLBC_TARGET_PLATFORM_ANDROID
 #ifndef LLBC_EXPORT
  #define LLBC_EXPORTING 1
  #define LLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef LLBC_HIDDEN
  #define LLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_ANDROID


