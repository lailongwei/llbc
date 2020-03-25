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
#include "llbc/core/thread/FastLock.h"
#include "llbc/core/os/OS_Console.h"

__LLBC_INTERNAL_NS_BEGIN
static LLBC_NS LLBC_FastLock __g_consoleLock[ 2 ];
__LLBC_INTERNAL_NS_END

#if LLBC_TARGET_PLATFORM_NON_WIN32
__LLBC_INTERNAL_NS_BEGIN
static int __g_consoleColor[ 2 ];
const static char* __g_consoleColorBeginFmt = "\033[";
const static char* __g_consoleHighColorBeginFmt = "\033[1;";
const static char* __g_consoleColorEndFmt = "\033[0m";

static const int __g_consoleFgColorNum = 8;
static const int __g_consoleBgColorNum = 8;
static const char* __g_ConsoleFgColorCode[ __g_consoleFgColorNum ] = { "30", "31", "32", "34", "33", "35", "36", "37" };
static const char* __g_ConsoleBgColorCode[ __g_consoleBgColorNum ] = { "40", "41", "42", "44", "43", "45", "46", "47" };

void __GetConsoleColorCode(int color, const char*& begFmt, const char*& fgColorCode, const char*& bgColorCode, const char*& endFmt)
{
    if (LIKELY(color <= 0))
        return;

    int fgColor = color & ((1 << __g_consoleFgColorNum) - 1);
    int bgColor = (color >> __g_consoleFgColorNum) & ((1 << __g_consoleBgColorNum) - 1);
    int highColor = (color >> __g_consoleFgColorNum) >> __g_consoleBgColorNum;
    if (LIKELY(fgColor > 0))
    {
        int idx = -1;
        while (++idx, fgColor = fgColor >> 1, fgColor > 0 && idx < __g_consoleFgColorNum);
        fgColorCode = __g_ConsoleFgColorCode[ idx ];
    }

    if (LIKELY(bgColor > 0))
    {
        int idx = -1;
        while (++idx, bgColor = bgColor >> 1, bgColor > 0 && idx < __g_consoleBgColorNum);
        bgColorCode = __g_ConsoleBgColorCode[ idx ];
    }

    begFmt = (highColor > 0) ? __g_consoleHighColorBeginFmt : __g_consoleColorBeginFmt;
    endFmt = __g_consoleColorEndFmt;
}

__LLBC_INTERNAL_NS_END
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

__LLBC_NS_BEGIN

int LLBC_GetConsoleColor(FILE *file)
{
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

#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_INTERNAL_NS __g_consoleLock[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ].Lock();
    const int color = LLBC_INTERNAL_NS __g_consoleColor[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ];
    LLBC_INTERNAL_NS __g_consoleLock[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ].Unlock();
    return color;
#else

    LLBC_INTERNAL_NS __g_consoleLock[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ].Lock();

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

#if LLBC_TARGET_PLATFORM_NON_WIN32
    LLBC_INTERNAL_NS __g_consoleLock[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ].Lock();
    LLBC_INTERNAL_NS __g_consoleColor[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ] = color;
    LLBC_INTERNAL_NS __g_consoleLock[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ].Unlock();
    return LLBC_OK;
#else

    LLBC_INTERNAL_NS __g_consoleLock[ (fileNo == 1 || fileNo == 2 ? 0 : 1) ].Lock();

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
	if (LIKELY(file == stdout || file == stderr))
	{
        const char* colorBegFmt = NULL;
        const char* colorEndFmt = NULL;
        const char* fgColorCode = NULL;
        const char* bgColorCode = NULL;
        LLBC_INTERNAL_NS __GetConsoleColorCode(LLBC_GetConsoleColor(file), colorBegFmt, fgColorCode, bgColorCode, colorEndFmt);
        if (LIKELY(fgColorCode != NULL && bgColorCode != NULL))
        {
            fprintf(file, (newline ? "%s%s;%sm%s%s\n" : "%s%s;%sm%s%s"), colorBegFmt, fgColorCode, bgColorCode, buf, colorEndFmt);
        }
        else if (LIKELY(fgColorCode != NULL || bgColorCode != NULL))
        {
            fprintf(file, (newline ? "%s%sm%s%s\n" : "%s%sm%s%s"), colorBegFmt, (fgColorCode != NULL ? fgColorCode : bgColorCode), buf, colorEndFmt);
        }
        else
        {
            fprintf(file, (newline ? "%s\n" : "%s"), buf);
        }
	}
	else
	{
        fprintf(file, (newline ? "%s\n" : "%s"), buf);

	}
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
