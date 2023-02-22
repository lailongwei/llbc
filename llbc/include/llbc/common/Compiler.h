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

// Compiler types macro define.
#define LLBC_COMP_MSVC  0  // Microsoft visual c++ compiler(cl)
#define LLBC_COMP_GCC   1  // GNU c compiler(gcc/g++)
#define LLBC_COMP_CLANG 2  // CLang compiler(clang)(for now, could not determine)
#define LLBC_COMP_OTHER 3  // Other compiles

// Determine current compiler.
// Microsoft visual c++ compiler.
// Version list:
//  MSVC++5.0   -> 1100
//  MSVC++6.0   -> 1200
//  MSVC++7.0   -> 1300
//  MSVC++7.1   -> 1310(Visual Studio 2003)
//  MSVC++8.0   -> 1400(Visual Studio 2005)
//  MSVC++9.0   -> 1500(Visual Studio 2008)
//  MSVC++10.0  -> 1600(Visual Studio 2010)
//  MSVC++11.0  -> 1700(Visual Studio 2012)
//  MSVC++12.0  -> 1800(Visual Studio 2013)
//  MSVC++14.0  -> 1900(Visual Studio 2015)
//  MSVC++15.0  -> 1910(Visual Studio 2017)
//  MSVC++16.0  -> 1920(Visual Studio 2019)
//  MSVC++17.0  -> 1930(Visual Studio 2022)
//  ... ...
#if LLBC_TARGET_PLATFORM_WIN32 && defined(_MSC_VER)
 #define LLBC_CUR_COMP          LLBC_COMP_MSVC
 #define LLBC_CUR_COMP_DESC     "MSC"
 #define LLBC_COMP_VER          _MSC_VER
 #define LLBC_COMP_MAJOR_VER    _MSC_VER
 #define LLBC_COMP_MINOR_VER    0
 #define LLBC_COMP_PATCH_LEVEL  0
#endif // LLBC_TARGET_PLATFORM_WIN32 && defined(_MSC_VER)

// GNU C compiler.
// The compiler version is converted, if gcc version is 3.2.0
// the version value is 30200
#if defined(__GNUC__) && !defined(__clang__)
 #define LLBC_CUR_COMP          LLBC_COMP_GCC
 #define LLBC_CUR_COMP_DESC     "gcc"
 #define LLBC_COMP_VER          (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
 #define LLBC_COMP_MAJOR_VER    __GNUC__
 #define LLBC_COMP_MINOR_VER    __GNUC_MINOR__
 #define LLBC_COMP_PATCH_LEVEL  __GNUC_PATCHLEVEL__
#endif

// CLang compiler.
// The compile version is converted, like GUN C compiler
#if defined(__clang__)
 #define LLBC_CUR_COMP          LLBC_COMP_CLANG
 #define LLBC_CUR_COMP_DESC     "clang"
 #define LLBC_COMP_VER          (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
 #define LLBC_COMP_MAJOR_VER    __clang_major__
 #define LLBC_COMP_MINOR_VER    __clang_minor__
 #define LLBC_COMP_PATCH_LEVEL  __clang_patchlevel__
#endif

// Other compiles.
#ifndef LLBC_CUR_COMP
 #warning "Cannot recognize the target compiler! please makesure you're using MSVC compiler or GNU c compiler or CLANG compiler to compiling llbc library!"
 #define LLBC_CUR_COMP          LLBC_COMP_OTHER
 #define LLBC_CUR_COMP_DESC     "other"
 #define LLBC_COMP_VER          0
 #define LLBC_COMP_MAJOR_VER    0
 #define LLBC_COMP_MINOR_VER    0
 #define LLBC_COMP_PATCH_LEVEL  0
 #endif


