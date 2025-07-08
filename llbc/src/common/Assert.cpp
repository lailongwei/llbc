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

#include "llbc/common/OSHeader.h"
#include "llbc/common/Assert.h"

#if LLBC_TARGET_PLATFORM_NON_WIN32
extern const char *__progname;
#else // Win32
static char __progname_full[MAX_PATH + 1];
#endif

__LLBC_NS_BEGIN

void __LLBC_AssertFail(const char *cond,
                       const char *file,
                       unsigned int line,
                       const char *func)
{
    // Win32 platform only: Get program name.
    #if LLBC_TARGET_PLATFORM_WIN32
    const char *__progname = __progname_full;
    const DWORD ret = ::GetModuleFileNameA(NULL, __progname_full, sizeof(__progname_full) - 1);
    if (UNLIKELY(ret == (sizeof(__progname_full) - 1) || ret == 0))
    {
        memcpy(__progname_full, "?????", 6);
    }
    else
    {
        __progname_full[ret] = '\0';
        DWORD pathSepPos = ret - 1;
        while (pathSepPos > 0)
        {
            if (__progname_full[pathSepPos] == '\\' ||
                __progname_full[pathSepPos] == '/')
            {
                __progname = &__progname_full[pathSepPos + 1];
                break;
            }

            --pathSepPos;
        }
    }
    #endif

    // Get file basename.
    #if LLBC_TARGET_PLATFORM_WIN32
    const char *fileBaseName = strrchr(file, '\\');
    if (!fileBaseName)
        fileBaseName = strrchr(file, '/');
    #else // Non-Win32
    const char *fileBaseName = strrchr(file, '/');
    #endif // Win32

    if (fileBaseName)
        ++fileBaseName;
    else
        fileBaseName = file;

    // Output assertion information to stderr.
    fprintf(stderr,
            "%s: %s:%u %s: Asseration `%s' failed.\n",
            __progname, fileBaseName, line, func, cond);
    fflush(stderr);

    // Raise SIGABRT signal.
    abort();
}

__LLBC_NS_END
