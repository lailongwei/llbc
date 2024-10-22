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

#include "csllbc/common/Export.h"

#include "csllbc/core/os/_OS_Console.h"

LLBC_BEGIN_C_DECL

void csllbc_Console_Trace(const char *value, int valueLen)
{
#ifdef LLBC_DEBUG 
    if (UNLIKELY(valueLen <= 0))
        return;

    if (valueLen < 128)
    {
        char nativeValue[128];
        memcpy(nativeValue, value, valueLen);
        nativeValue[valueLen] = '\0';

        trace("%s", nativeValue);
    }
    else
    {
        char *nativeValue = LLBC_Malloc(char, valueLen + 1);
        memcpy(nativeValue, value, valueLen);

        nativeValue[valueLen] = '\0';
        trace("%s", nativeValue);
        free(nativeValue);
    }
#endif // LLBC_DEBUG
}

void csllbc_Console_SafePrint(bool toStdout, bool newLine, const char *value, int valueLen)
{
    if (UNLIKELY(valueLen == 0))
    {
        if (!newLine)
            return;

        value = "";
    }

    FILE *file = toStdout ? stdout : stderr;
    const char *fmtter = newLine ? "%s\n" : "%s";

    if (valueLen < 128)
    {
        char nativeValue[128];
        if (valueLen > 0)
            memcpy(nativeValue, value, valueLen);

        nativeValue[valueLen] = '\0';
        LLBC_FilePrint(file, fmtter, nativeValue);
    }
    else
    {
        char *nativeValue = LLBC_Malloc(char, valueLen + 1);
        memcpy(nativeValue, value, valueLen);

        nativeValue[valueLen] = '\0';
        LLBC_FilePrint(file, fmtter, nativeValue);
        free(nativeValue);
    }
}

int csllbc_Console_GetForegroundColor()
{
    int color = LLBC_GetConsoleColor(stdout);
    if (UNLIKELY(color == LLBC_FAILED))
        return LLBC_FAILED;

    return color & 0xf;
}

int csllbc_Console_SetForegroundColor(int color)
{
    int oldColor = LLBC_GetConsoleColor(stdout);
    if (UNLIKELY(oldColor == LLBC_FAILED))
        return LLBC_FAILED;

    int oldFgColor = oldColor & 0xf;
    if (color == oldFgColor)
        return LLBC_OK;

    int newColor = (oldColor & ~0xf) | (color & 0xf);
    return LLBC_SetConsoleColor(stdout, newColor);
}

int csllbc_Console_GetBackgroundColor()
{
    int color = LLBC_GetConsoleColor(stdout);
    if (UNLIKELY(color == LLBC_FAILED))
        return LLBC_FAILED;

    return (color & 0xf0) >> 4;
}

int csllbc_Console_SetBackgroundColor(int color)
{
    int oldColor = LLBC_GetConsoleColor(stdout);
    if (UNLIKELY(oldColor == LLBC_FAILED))
        return LLBC_FAILED;

    int oldBgColor = (oldColor & 0xf0) >> 4;
    if (color == oldBgColor)
        return LLBC_OK;

    int newColor = (oldColor & ~0xf0) | ((color << 4) & 0xf0);
    return LLBC_SetConsoleColor(stdout, newColor);
}

int csllbc_Console_ResetColor()
{
    return LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Default);
}

int csllbc_Console_SafeFlush(bool flushStdout)
{
    return LLBC_FlushFile(flushStdout ? stdout : stderr);
}

LLBC_END_C_DECL
