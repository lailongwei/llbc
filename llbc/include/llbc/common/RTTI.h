/**
 * @file    RTTI.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/09
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_RTTI_H__
#define __LLBC_COM_RTTI_H__

#include "llbc/common/Macro.h"
#include "llbc/common/OSHeader.h"

__LLBC_NS_BEGIN

#if LLBC_TARGET_PLATFORM_WIN32
/**
 * Get type name(demangled).
 */
#define LLBC_GetTypeName(ty)                                                                \
     (LLBC_String(typeid(ty).name()).findreplace("class ", "").findreplace("struct ", "")   \
      .findreplace(" *", "").findreplace("`anonymous namespace'", "(anonymous namespace)")) \

#else // Non-Win32

/**
 * Demangle cxx type name(only available in non-windows platform).
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String __LLBC_CxxDemangle(const char *name);

/**
 * Get type name(demangled).
 */
#define LLBC_GetTypeName(ty) __LLBC_CxxDemangle(typeid(ty).name)

#endif // Win32

__LLBC_NS_END

#endif // !__LLBC_COM_RTTI_H__
