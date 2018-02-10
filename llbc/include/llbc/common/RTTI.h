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

#ifndef __LLBC_COM_RTTI_H__
#define __LLBC_COM_RTTI_H__

#include "llbc/common/Macro.h"
#include "llbc/common/OSHeader.h"
#include "llbc/common/StringDataType.h"

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
#define LLBC_GetTypeName(ty) __LLBC_CxxDemangle(typeid(ty).name())

#endif // Win32

__LLBC_NS_END

#endif // !__LLBC_COM_RTTI_H__
