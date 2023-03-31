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

#include "llbc/core/file/File.h"
#include "llbc/core/thread/FastLock.h"
#include "llbc/core/os/OS_Console.h"

__LLBC_INTERNAL_NS_BEGIN
static LLBC_NS LLBC_FastLock __g_consoleLock[2];
__LLBC_INTERNAL_NS_END

#if LLBC_TARGET_PLATFORM_NON_WIN32
__LLBC_INTERNAL_NS_BEGIN
static LLBC_THREAD_LOCAL int __g_consoleColor[2];
const static char *__g_consoleColorBeginFmt = "\033[";
const static char *__g_consoleColorEndFmt = "\033[0m";

static const int __g_consoleFgColorNum = 8;
static const int __g_consoleBgColorNum = 8;
static const char* __g_ConsoleColorCode[2][__g_consoleFgColorNum + __g_consoleBgColorNum + 1] = {
    { "", "34", "32", "36", "31", "35", "33", "37", "30", "44", "42", "46", "41", "45", "43", "47", "40" },
    { "", ";34", ";32", ";36", ";31", ";35", ";33", ";37", ";30", ";44", ";42", ";46", ";41", ";45", ";43", ";47", ";40" }
};

static const int __g_consoleColorFmtLen = 11;
void __GetConsoleColorCode(int color, char fmt[__g_consoleColorFmtLen])
{
    if (LIKELY(color <= 0))
        return;

    const int fgColor = color & 0xf;
    const int bgColor = ((color & 0xf0) >> 4);
    const bool high = (color & 0xf00) >> 8;
    const static int beginLen = strlen(__g_consoleColorBeginFmt);
    int idx = beginLen;

    memcpy(fmt, __g_consoleColorBeginFmt, beginLen);

    if (high)  // highlight
        fmt[idx++] = '1';

    const static int pre2Len[2] = { 2, 3 };
    if (fgColor > 0)  // front color
    {
        const int pre = high ? 1 : 0;
        const int len = pre2Len[pre];
        memcpy(fmt + idx, __g_ConsoleColorCode[pre][fgColor], len);
        idx += len;
    }

    if (bgColor > 0)  // back color
    {
        const int pre = (high || fgColor > 0) ? 1 : 0;
        const int len = pre2Len[pre];
        memcpy(fmt + idx, __g_ConsoleColorCode[pre][__g_consoleFgColorNum + bgColor], len);
        idx += len;
    }
    fmt[idx] = 'm';
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
    return LLBC_INTERNAL_NS __g_consoleColor[(fileNo == 1 || fileNo == 2 ? 0 : 1)];
#else
    HANDLE handle = (fileNo == 1 ? ::GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE));
    LLBC_FastLock &lock = LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)];
    lock.Lock();

    CONSOLE_SCREEN_BUFFER_INFO info;
    if (::GetConsoleScreenBufferInfo(handle, &info) == 0)
    {
        lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    lock.Unlock();
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
    LLBC_INTERNAL_NS __g_consoleColor[(fileNo == 1 || fileNo == 2 ? 0 : 1)] = color;
    return LLBC_OK;
#else
    HANDLE handle = (fileNo == 1 ? ::GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE));
    LLBC_FastLock &lock = LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)];
    lock.Lock();

    if (::SetConsoleTextAttribute(handle, color) == 0)
    {
        lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_OSAPI);
        return LLBC_FAILED;
    }

    lock.Unlock();
    return LLBC_OK;
#endif
}

int __LLBC_FilePrint(bool newline, FILE *file, const char *fmt, ...)
{
    const int fileNo = LLBC_File::GetFileNo(file);
    LLBC_ReturnIf(UNLIKELY(fileNo == -1), LLBC_FAILED);

    char stackBuf[512 + 128 + 1];

    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(stackBuf, sizeof(stackBuf), fmt, ap);
    va_end(ap);

    LLBC_SetErrAndReturnIf(UNLIKELY(len < 0), LLBC_ERROR_CLIB, LLBC_FAILED);

    char *buf = stackBuf;
    if (len >= static_cast<int>(sizeof(stackBuf)))
    {
        buf = LLBC_Malloc(char, len + 1);

        va_start(ap, fmt);
        len = vsnprintf(buf, len + 1, fmt, ap);
        va_end(ap);

        if (UNLIKELY(len < 0))
        {
            free(buf);
            LLBC_SetLastError(LLBC_ERROR_CLIB);

            return LLBC_FAILED;
        }
    }

    #if LLBC_TARGET_PLATFORM_NON_WIN32
    flockfile(file);
    bool fmtPrint = false;
    if (file == stdout || file == stderr)
    {
        const int color = LLBC_GetConsoleColor(file);
        if (color > 0)
        {
            fmtPrint = true;
            char colorFmt[LLBC_INTERNAL_NS __g_consoleColorFmtLen] = {};
            LLBC_INTERNAL_NS __GetConsoleColorCode(color, colorFmt);
            fprintf(file, (newline ? "%s%s%s\n" : "%s%s%s"), colorFmt, buf, LLBC_INTERNAL_NS __g_consoleColorEndFmt);
        }
    }

    if (!fmtPrint)
    {
        fprintf(file, (newline ? "%s\n" : "%s"), buf);
    }
    funlockfile(file);
    #else // Win32
    const int clrIdx = (fileNo == 1 || fileNo == 2 ? 0 : 1);
    LLBC_FastLock &lock = LLBC_INTERNAL_NS __g_consoleLock[clrIdx];

    lock.Lock();
    fprintf(file, newline ? "%s\n" : "%s", buf);
    lock.Unlock();
    #endif // !Non-Win32

    LLBC_DoIf(len >= static_cast<int>(sizeof(stackBuf)), free(buf));

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
    if (UNLIKELY(fflush(file) != 0))
    {
        funlockfile(file);
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    funlockfile(file);
    return LLBC_OK;
#else // Win32
    const int fileNo = LLBC_File::GetFileNo(file);
    LLBC_FastLock &lock = LLBC_INTERNAL_NS __g_consoleLock[(fileNo == 1 || fileNo == 2 ? 0 : 1)];
    lock.Lock();

    if (UNLIKELY(fflush(file) != 0))
    {
        lock.Unlock();
        LLBC_SetLastError(LLBC_ERROR_CLIB);
        return LLBC_FAILED;
    }

    lock.Unlock();
    return LLBC_OK;
#endif
}

__LLBC_NS_END
