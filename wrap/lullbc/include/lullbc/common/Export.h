/**
 * @file    Export.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017/07-06
 * @version 1.0
 *
 * @brief
 */
#ifndef __LULLBC_COM_EXPORT_H__
#define __LULLBC_COM_EXPORT_H__

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

#endif // !__LULLBC_COM_EXPORT_H__
