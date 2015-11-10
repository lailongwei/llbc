/**
 * @file    Compiler.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/26
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_COMPILE_H__
#define __LLBC_COM_COMPILE_H__

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
//  ... ...
#if LLBC_TARGET_PLATFORM_WIN32 && defined(_MSC_VER)
 #define LLBC_CUR_COMP          LLBC_COMP_MSVC
 #define LLBC_CUR_COMP_DESC     "MSC"
 #define LLBC_COMP_VER          _MSC_VER
 #define LLBC_COMP_MAJOR_VER    _MSC_VER
 #define LLBC_COMP_MINOR_VER    0
 #define LLBC_COMP_PATCH_LEVEL  0
#endif // LLLBC_TARGET_PLATFORM_WIN32 && defined(_MSC_VER)

// GNU C compiler.
// The compiler version is converted, if gcc version is 3.2.0
// the version value is 30200
#ifdef __GNUC__
 #define LLBC_CUR_COMP          LLBC_COMP_GCC
 #define LLBC_CUR_COMP_DESC     "gcc"
 #define LLBC_COMP_VER          (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
 #define LLBC_COMP_MAJOR_VER    __GNUC__
 #define LLBC_COMP_MINOR_VER    __GNUC_MINOR__
 #define LLBC_COMP_PATCH_LEVEL  __GNUC_PATCHLEVEL__
#endif

// Other compiles.
#ifndef LLBC_CUR_COMP
 #warning "Cannot recognize the target compiler! please makesure you're using MSVC compiler or GNU c compiler to compiling llbc library!"
 #define LLBC_CUR_COMP          LLBC_COMP_OTHER
 #define LLBC_CUR_COMP_DESC     "other"
 #define LLBC_COMP_VER          0
 #define LLBC_COMP_MAJOR_VER    0
 #define LLBC_COMP_MINOR_VER    0
 #define LLBC_COMP_PATCH_LEVEL  0
#endif

#endif // !__LLBC_COM_COMPILE_H__
