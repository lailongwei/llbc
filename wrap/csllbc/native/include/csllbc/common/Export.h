/**
 * @file    Export.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_COM_EXPORT_H__
#define __CSLLBC_COM_EXPORT_H__

#include "llbc/common/PFConfig.h"

// Define CSLLBC_EXPORT to __declspec(dllexport), win32 platform specific.

// LINUX
#if LLBC_TARGET_PLATFORM_LINUX
 #ifndef CSLLBC_EXPORT
  #define CSLLBC_EXPORTING 1
  #define CSLLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef CSLLBC_HIDDEN
  #define CSLLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_LINUX

// WIN32
#if LLBC_TARGET_PLATFORM_WIN32
 #ifndef CSLLBC_EXPORT
  #define CSLLBC_EXPORTING 1
  #define CSLLBC_EXPORT __declspec(dllexport)
 #endif
 #ifndef CSLLBC_HIDDEN
  #define CSLLBC_HIDDEN
 #endif
#endif // LLBC_TARGET_PLATFORM_WIN32

// IPHONE
#if LLBC_TARGET_PLATFORM_IPHONE
 #ifndef CSLLBC_EXPORT		
  #define CSLLBC_EXPORTING 1
  #define CSLLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef CSLLBC_HIDDEN
  #define CSLLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_IPHONE

// MAC
#if LLBC_TARGET_PLATFORM_MAC
 #ifndef CSLLBC_EXPORT
  #define CSLLBC_EXPORTING 1
  #define CSLLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef CSLLBC_HIDDEN
  #define CSLLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_MAC

// ANDROID
#if LLBC_TARGET_PLATFORM_ANDROID
 #ifndef CSLLBC_EXPORT
  #define CSLLBC_EXPORTING 1
  #define CSLLBC_EXPORT __attribute__ ((__visibility__("default")))
 #endif
 #ifndef CSLLBC_HIDDEN
  #define CSLLBC_HIDDEN __attribute__ ((__visibility__("hidden")))
 #endif
#endif // LLBC_TARGET_PLATFORM_ANDROID

#endif // !__CSLLBC_COM_EXPORT_H__
