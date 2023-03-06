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

// ==================================================================================
// Platform specific macros pre-define.
//
// If in macos/ios platform, include macos/ios platform recognize implement file.
#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

// If in windows platform, define WIN32 macro
#ifndef WIN32
 #if defined(_WIN32)
  #define WIN32
 #endif // _WIN32
#endif // !WIN32

// If in linux platform, define LINUX macro
#ifndef LINUX
 #if defined(__linux__)
  #define LINUX
 #endif // __linux__
#endif // !LINUX

// ==================================================================================
// Platform type enumeration.
//
#define LLBC_PLATFORM_UNKNOWN       0
#define LLBC_PLATFORM_LINUX         1
#define LLBC_PLATFORM_WIN32         2
#define LLBC_PLATFORM_IPHONE        3
#define LLBC_PLATFORM_MAC           4
#define LLBC_PLATFORM_ANDROID       5

#define LLBC_TARGET_PLATFORM        LLBC_PLATFORM_UNKNOWN

// ==================================================================================
// Recognize platforms
//
// Linux platform recognize.
#if !LLBC_TARGET_PLATFORM && defined(LINUX) && !defined(__ANDROID__)
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_LINUX
#endif

// Windows platform recognize.
#if !LLBC_TARGET_PLATFORM && defined(WIN32)
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_WIN32
#endif

// iPhone(included Simulator) platform recognize.
#if !LLBC_TARGET_PLATFORM && TARGET_OS_IPHONE
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_IPHONE
#endif

// Mac platform recognize.
#if !LLBC_TARGET_PLATFORM && TARGET_OS_MAC
#undef LLBC_TARGET_PLATFORM
#define LLBC_TARGET_PLATFORM LLBC_PLATFORM_MAC
#endif

// Android platform recognize.
#if !LLBC_TARGET_PLATFORM && defined(__ANDROID__)
 #undef LLBC_TARGET_PLATFORM
 #define LLBC_TARGET_PLATFORM LLBC_PLATFORM_ANDROID
#endif

// Lastest, check LLBC_TARGET_PLATFORM macro set or not.
#if !LLBC_TARGET_PLATFORM
 #error "Cannot recognize the target platform; are you targeting an unsuported platform?"
#endif

// Linux/Non-Linux platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_LINUX
 #define LLBC_TARGET_PLATFORM_LINUX                         1
 #define LLBC_TARGET_PLATFORM_NON_LINUX                     0
#else
 #define LLBC_TARGET_PLATFORM_LINUX                         0
 #define LLBC_TARGET_PLATFORM_NON_LINUX                     1
#endif

// WIN32/Non-WIN32 platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_WIN32
 #define LLBC_TARGET_PLATFORM_WIN32                         1
 #define LLBC_TARGET_PLATFORM_NON_WIN32                     0
#else
 #define LLBC_TARGET_PLATFORM_WIN32                         0
 #define LLBC_TARGET_PLATFORM_NON_WIN32                     1
#endif

// Iphone/Non-Iphone platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_IPHONE
 #define LLBC_TARGET_PLATFORM_IPHONE                        1
 #define LLBC_TARGET_PLATFORM_NON_IPHONE                    0
 #if TARGET_IPHONE_SIMULATOR
  #define LLBC_TARGET_PLATFORM_IPHONE_SIMULATOR             1
 #else
  #define LLBC_TARGET_PLATFORM_IPHONE_SIMULATOR             0
 #endif
#else
 #define LLBC_TARGET_PLATFORM_IPHONE                        0
 #define LLBC_TARGET_PLATFORM_IPHONE_SIMULATOR              0
 #define LLBC_TARGET_PLATFORM_NON_IPHONE                    1
#endif

// Mac/Non-Mac platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_MAC
 #define LLBC_TARGET_PLATFORM_MAC                           1
 #define LLBC_TARGET_PLATFORM_NON_MAC                       0
#else
 #define LLBC_TARGET_PLATFORM_MAC                           0
 #define LLBC_TARGET_PLATFORM_NON_MAC                       1
#endif

// Android/Non-Android platform macro define.
#if LLBC_TARGET_PLATFORM == LLBC_PLATFORM_ANDROID
 #define LLBC_TARGET_PLATFORM_ANDROID                       1
 #define LLBC_TARGET_PLATFORM_NON_ANDROID                   0
#else
 #define LLBC_TARGET_PLATFORM_ANDROID                       0
 #define LLBC_TARGET_PLATFORM_NON_ANDROID                   1
#endif

// ==================================================================================
// Processer type enumeration.
//
#define LLBC_PROCESSOR_UNKNOWN                              0
#define LLBC_PROCESSOR_X86                                  1
#define LLBC_PROCESSOR_X86_64                               2
#define LLBC_PROCESSOR_ARM                                  3
#define LLBC_PROCESSOR_ARM_THUMB                            4
#define LLBC_PROCESSOR_ARM_64                               5

// ==================================================================================
// Recognize processors
// About processor predefined macros, see:
// All mainstream compilers: https://sourceforge.net/p/predef/wiki/Architectures/
// MSVC only:                https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
//
#if LLBC_TARGET_PLATFORM_WIN32

 #if defined(_M_IX86)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_X86
 #elif defined(_M_X64) || defined(_M_AMD64)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_X86_64
 #elif defined(_M_ARM)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_ARM
 #elif defined(_M_ARMT)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_ARM_THUMB
 #elif defined(_M_ARM64)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_ARM_64
 #else // Unknown
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_UNKNOWN
 #endif // defined(_M_IX86)

#else // LLBC_TARGET_PLATFORM_NON_WIN32

 #if defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_X86
 #elif defined(__x86_64__) || defined(__amd64__)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_X86_64
 #elif defined(__arm__)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_ARM
 #elif defined(__arm__) && defined(__thumb__)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_ARM_THUMB
 #elif defined(__aarch64__)
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_ARM_64
 #else // Unknown
  #define LLBC_TARGET_PROCESSOR LLBC_PROCESSOR_UNKNOWN
 #endif // defined(__ix86__)

#endif // LLBC_TARGET_PLATFORM_WIN32

#if LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_X86
 #define LLBC_16BIT_PROCESSOR                               0
 #define LLBC_32BIT_PROCESSOR                               1
 #define LLBC_64BIT_PROCESSOR                               0
 #define LLBC_TARGET_PROCESSOR_X86                          1
 #define LLBC_TARGET_PROCESSOR_X86_64                       0
 #define LLBC_TARGET_PROCESSOR_ARM                          0
 #define LLBC_TARGET_PROCESSOR_ARM_THUMB                    0
 #define LLBC_TARGET_PROCESSOR_ARM_64                       0
 #define LLBC_TARGET_PROCESSOR_DESC                         "x86"
#elif LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_X86_64
 #define LLBC_16BIT_PROCESSOR                               0
 #define LLBC_32BIT_PROCESSOR                               0
 #define LLBC_64BIT_PROCESSOR                               1
 #define LLBC_TARGET_PROCESSOR_X86                          0
 #define LLBC_TARGET_PROCESSOR_X86_64                       1
 #define LLBC_TARGET_PROCESSOR_ARM                          0
 #define LLBC_TARGET_PROCESSOR_ARM_THUMB                    0
 #define LLBC_TARGET_PROCESSOR_ARM_64                       0
 #define LLBC_TARGET_PROCESSOR_DESC                         "x86_64"
#elif LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_ARM
 #define LLBC_16BIT_PROCESSOR                               0
 #define LLBC_32BIT_PROCESSOR                               1
 #define LLBC_64BIT_PROCESSOR                               0
 #define LLBC_TARGET_PROCESSOR_X86                          0
 #define LLBC_TARGET_PROCESSOR_X86_64                       0
 #define LLBC_TARGET_PROCESSOR_ARM                          1
 #define LLBC_TARGET_PROCESSOR_ARM_THUMB                    0
 #define LLBC_TARGET_PROCESSOR_ARM_64                       0
 #define LLBC_TARGET_PROCESSOR_DESC                         "arm"
#elif LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_ARM_THUMB
 #define LLBC_16BIT_PROCESSOR                               1
 #define LLBC_32BIT_PROCESSOR                               0
 #define LLBC_64BIT_PROCESSOR                               0
 #define LLBC_TARGET_PROCESSOR_X86                          0
 #define LLBC_TARGET_PROCESSOR_X86_64                       0
 #define LLBC_TARGET_PROCESSOR_ARM                          0
 #define LLBC_TARGET_PROCESSOR_ARM_THUMB                    1
 #define LLBC_TARGET_PROCESSOR_ARM_64                       0
 #define LLBC_TARGET_PROCESSOR_DESC                         "thumb"
#elif LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_ARM_64
 #define LLBC_16BIT_PROCESSOR                               0
 #define LLBC_32BIT_PROCESSOR                               0
 #define LLBC_64BIT_PROCESSOR                               1
 #define LLBC_TARGET_PROCESSOR_X86                          0
 #define LLBC_TARGET_PROCESSOR_X86_64                       0
 #define LLBC_TARGET_PROCESSOR_ARM                          0
 #define LLBC_TARGET_PROCESSOR_ARM_THUMB                    0
 #define LLBC_TARGET_PROCESSOR_ARM_64                       1
 #define LLBC_TARGET_PROCESSOR_DESC                         "arm64"
#else // Unknown processor
 #error "Cannot recognize the target processor; are you targeting an unsuported processor?"
#endif // LLBC_TARGET_PROCESSOR == LLBC_PROCESSOR_X86



