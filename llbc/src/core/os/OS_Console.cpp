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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/file/File.h"
#if LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/thread/FastLock.h"
#endif // LLBC_TARGET_PLATFORM_WIN32
#include "llbc/core/os/OS_Console.h"

#if LLBC_TARGET_PLATFORM_WIN32
__LLBC_INTERNAL_NS_BEGIN
static LLBC_NS LLBC_FastLock __g_consoleLock[2];
__LLBC_INTERNAL_NS_END
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

int LLBC_GetConsoleColor(FILE *file)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else
    const int fileNo = LLBC_File::GetFileNo(file);
    if (UNLIKELY(fileNo == -1))
    {
        return LLBC_FAILED;
    }
    else if (UNLIKELY(fileNo != 1 && fileNo != 2))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Lock();

    HANDLE handle = (fileNo == 1 ? 
        ::GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE));
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (::GetConsoleScreenBufferInfo(handle, &info) == 0)
    {
        LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();

    return info.wAttributes;
#endif
}

int LLBC_SetConsoleColor(FILE *file, int color)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
#else
    const int fileNo = LLBC_File::GetFileNo(file);
    if (UNLIKELY(fileNo == -1))
    {
        return LLBC_FAILED;
    }
    else if (UNLIKELY(fileNo != 1 && fileNo != 2))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Lock();

    HANDLE handle = (fileNo == 1 ? 
        ::GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE));
    if (::SetConsoleTextAttribute(handle, color) == 0)
    {
        LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }
    
    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();
    return LLBC_OK;
#endif
}

int __LLBC_FilePrint(bool newline, FILE *file, const char *fmt, ...)
{
    const int fileNo = LLBC_File::GetFileNo(file);
    if (UNLIKELY(fileNo == -1))
    {
        return LLBC_FAILED;
    }

    char *buf; int len;
    LLBC_FormatArg(fmt, buf, len);

#if LLBC_TARGET_PLATFORM_NON_WIN32
    flockfile(file);
    fprintf(file, (newline?"%s\n":"%s"), buf);
    fflush(file);
    funlockfile(file);
#else
    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Lock();
    fprintf(file, newline?"%s\n":"%s", buf);
    fflush(file);
    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();
#endif

    LLBC_Free(buf);

    return LLBC_OK;
}

int LLBC_FlushFile(FILE *file)
{
    if (UNLIKELY(!file))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    flockfile(file);
    if (UNLIKELY(::fflush(file) != 0))
    {
        funlockfile(file);
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    funlockfile(file);
    return LLBC_OK;
#else // Win32
    const int fileNo = LLBC_File::GetFileNo(file);
    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Lock();
    if (UNLIKELY(::fflush(file) != 0))
    {
        LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)].Unlock();
    return LLBC_OK;
#endif
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
