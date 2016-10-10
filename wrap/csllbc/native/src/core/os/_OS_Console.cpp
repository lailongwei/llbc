/**
 * @file    _OS_Console.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/22
 * @version 1.0
 *
 * @brief
 */

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
        ::memcpy(nativeValue, value, valueLen);
        nativeValue[valueLen] = '\0';

        trace("%s", nativeValue);
    }
    else
    {
        char *nativeValue = LLBC_Malloc(char, valueLen + 1);
        ::memcpy(nativeValue, value, valueLen);

        nativeValue[valueLen] = '\0';
        trace("%s", nativeValue);
        LLBC_Free(nativeValue);
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
            ::memcpy(nativeValue, value, valueLen);

        nativeValue[valueLen] = '\0';
        LLBC_FilePrint(file, fmtter, nativeValue);
    }
    else
    {
        char *nativeValue = LLBC_Malloc(char, valueLen + 1);
        ::memcpy(nativeValue, value, valueLen);

        nativeValue[valueLen] = '\0';
        LLBC_FilePrint(file, fmtter, nativeValue);
        LLBC_Free(nativeValue);
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
