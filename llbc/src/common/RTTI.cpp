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

#if LLBC_TARGET_PLATFORM_NON_WIN32
 #include <cxxabi.h>
#endif

#include "llbc/common/LibTlsInl.h"
#include "llbc/common/RTTI.h"

__LLBC_NS_BEGIN

#define __LLBC_GET_TYPE_NAME_Trim(str, len)      \
    it = rtti;                                   \
    skipCopy = 0;                                \
    itEnd = rtti + rawTyNameLen - totalSkipCopy; \
    while ((it = strstr(it, str)))               \
    {                                            \
        size_t copyLen = itEnd - it - len;       \
        memmove(it, it + len, copyLen);          \
        itEnd -= len;                            \
        skipCopy += len;                         \
        if (copyLen == 0)                        \
            break;                               \
    }                                            \
    totalSkipCopy += skipCopy;                   \
    rtti[rawTyNameLen - totalSkipCopy] = '\0'    \

const char *__LLBC_GetTypeName(const char *rawTyName)
{
#if LLBC_TARGET_PLATFORM_WIN32
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    char *rtti = tls->commonTls.rtti;
    size_t rawTyNameLen = strlen(rawTyName);
    memcpy(rtti, rawTyName, rawTyNameLen);
    rtti[rawTyNameLen] = '\0';

    size_t totalSkipCopy = 0;

    char *it;
    char *itEnd;
    size_t skipCopy;
    __LLBC_GET_TYPE_NAME_Trim("class ", 6);
    __LLBC_GET_TYPE_NAME_Trim("struct ", 7);
    __LLBC_GET_TYPE_NAME_Trim(" *", 2);

    char *anonBeg = rtti;
    while ((anonBeg = strchr(anonBeg, '`')))
    {
        *anonBeg = '(';
        char *anonEnd = strchr(anonBeg + 1, '\'');
        *anonEnd = ')';
    }

    return rtti;
#else // Non-Win32
    return __LLBC_CxxDemangle(rawTyName);
#endif // LLBC_TARGET_PLATFORM_WIN32
}

#undef __LLBC_GET_TYPE_NAME_Trim

#if LLBC_TARGET_PLATFORM_NON_WIN32

const char *__LLBC_CxxDemangle(const char *name)
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
        return "";

    int status = 0;
    size_t length = sizeof(libTls->commonTls.rtti);

    abi::__cxa_demangle(name, libTls->commonTls.rtti, &length, &status);
    if (status != 0)
        return "";

    return libTls->commonTls.rtti;
}

#endif // Non-Win32

__LLBC_NS_END
