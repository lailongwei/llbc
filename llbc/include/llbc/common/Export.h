/**
 * @file    Export.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_EXPORT_H__
#define __LLBC_COM_EXPORT_H__

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

#endif // !__LLBC_COM_EXPORT_H__
