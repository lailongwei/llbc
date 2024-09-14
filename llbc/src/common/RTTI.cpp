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

#include "llbc/common/Export.h"

#include "llbc/common/LibTlsInl.h"
#include "llbc/common/RTTI.h"

#if LLBC_TARGET_PLATFORM_NON_WIN32
#include <cxxabi.h>
#endif // Non-Win32

__LLBC_NS_BEGIN

const char *__LLBC_GetTypeName(const char *mangledTypeName)
{
    #if LLBC_TARGET_PLATFORM_WIN32
    const char *spacePos = strchr(mangledTypeName, ' ');
    return LIKELY(spacePos) ? spacePos + 1 : mangledTypeName;
    #else // Non-Win32
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
        return nullptr;

    int status;
    size_t len = sizeof(libTls->commonTls.rtti);
    ::abi::__cxa_demangle(mangledTypeName, libTls->commonTls.rtti, &len, &status);
    return UNLIKELY(status != 0) ? mangledTypeName : libTls->commonTls.rtti;
    #endif // Win32
}

const char *__LLBC_GetCompName(const char *mangledCompName)
{
    #if LLBC_TARGET_PLATFORM_WIN32
    const char *spacePos = strchr(mangledCompName, ' ');
    if (UNLIKELY(!spacePos))
        return mangledCompName;

    mangledCompName = spacePos + 1;
    const char *colonPos = strrchr(mangledCompName, ':');
    return colonPos ? colonPos + 1 : mangledCompName;
    #else // Non-Win32
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
        return nullptr;

    int status;
    size_t len = sizeof(libTls->commonTls.rtti);
    abi::__cxa_demangle(mangledCompName, libTls->commonTls.rtti, &len, &status);
    if (UNLIKELY(status != 0))
        return mangledCompName;

    const char *colonPos = strrchr(libTls->commonTls.rtti, ':');
    if (colonPos)
        memmove(libTls->commonTls.rtti, colonPos + 1, strlen(colonPos + 1) + 1);

    return libTls->commonTls.rtti;
    #endif // Win32
}

__LLBC_NS_END

