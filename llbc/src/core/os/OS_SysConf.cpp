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

#include "llbc/core/os/OS_SysConf.h"

__LLBC_NS_BEGIN

sint64 LLBC_pageSize = 0;

int __LLBC_InitSysConf()
{
#if LLBC_TARGET_PLATFORM_LINUX
    errno = 0;
    int tmpValue = sysconf(_SC_PAGESIZE);
    if (UNLIKELY(tmpValue == -1))
    {
        if (UNLIKELY(errno == 0))
            errno = EINVAL;

        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }
    LLBC_pageSize = tmpValue;
#elif LLBC_TARGET_PLATFORM_WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    LLBC_pageSize = si.dwPageSize;
#endif

    return LLBC_OK;
}

void __LLBC_CleanUpSysConf()
{
    LLBC_pageSize = 0;
}

__LLBC_NS_END