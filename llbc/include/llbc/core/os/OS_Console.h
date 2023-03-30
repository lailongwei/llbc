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

#pragma once

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief console color values enumeration.
 */
class LLBC_EXPORT LLBC_ConsoleColor
{
public:
    enum
    {
        #if LLBC_TARGET_PLATFORM_NON_WIN32
        Fg_Black = 8,
        Fg_Red = 4,
        Fg_Green = 2,
        Fg_Blue = 1,
        Fg_Yellow = Fg_Green | Fg_Red,
        Fg_Purple = Fg_Red | Fg_Blue,
        Fg_Cyan = Fg_Green | Fg_Blue,
        Fg_White = Fg_Green | Fg_Blue | Fg_Red,

        Bg_Black = Fg_Black << 4,
        Bg_Red = Fg_Red << 4,
        Bg_Green = Fg_Green << 4,
        Bg_Blue = Fg_Blue << 4,
        Bg_Yellow = Fg_Yellow << 4,
        Bg_Purple = Fg_Purple << 4,
        Bg_Cyan = Fg_Cyan << 4,
        Bg_White = Fg_White << 4,

        Highlight_Fg = 1 << 8,
        Highlight_Bg = Highlight_Fg,

        Fg_Default = 0,
        Bg_Default = 0
        #else // LLBC_TARGET_PLATFORM_WIN32
        Fg_Black  = 0,
        Fg_Red    = FOREGROUND_RED,
        Fg_Green  = FOREGROUND_GREEN,
        Fg_Blue   = FOREGROUND_BLUE,
        Fg_Yellow = FOREGROUND_RED  | FOREGROUND_GREEN,
        Fg_Purple = FOREGROUND_RED  | FOREGROUND_BLUE,
        Fg_Cyan   = FOREGROUND_BLUE | FOREGROUND_GREEN,
        Fg_White  = FOREGROUND_RED  | FOREGROUND_GREEN | FOREGROUND_BLUE,
        
        Bg_Black  = 0,
        Bg_Red    = BACKGROUND_RED,
        Bg_Green  = BACKGROUND_GREEN,
        Bg_Blue   = BACKGROUND_BLUE,
        Bg_Yellow = BACKGROUND_RED  | BACKGROUND_GREEN,
        Bg_Purple = BACKGROUND_RED  | BACKGROUND_BLUE,
        Bg_Cyan   = BACKGROUND_BLUE | BACKGROUND_GREEN,
        Bg_White  = BACKGROUND_RED  | BACKGROUND_GREEN | BACKGROUND_BLUE,

        Highlight_Fg = FOREGROUND_INTENSITY,
        Highlight_Bg = BACKGROUND_INTENSITY,

        Fg_Default = Fg_White,
        Bg_Default = Bg_Black
        #endif // LLBC_TARGET_PLATFORM_NON_WIN32
    };
};

/**
 * Get console color value.
 * @param[in] file - console type, can set to stdout or stderr.
 * @return int - console color, if return -1, it means error occurred.
 */
LLBC_EXPORT int LLBC_GetConsoleColor(FILE *file);

/**
 * Set console color value.
 * @param[in] file  - console type, can set to stdout or stderr.
 * @param[in] color - color value.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_SetConsoleColor(FILE *file, int color);

/**
 * Secure output message to console.
 * @param[in] newline - the new line flag, if specific, will auto append NewLine character(s).
 * @param[in] file    - console type, can set to stdout or stderr.
 * @param[in] fmt     - format control string.
 * @param[in] ...     - option arguments.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int __LLBC_FilePrint(bool newline,
                                 FILE *file,
                                 const char *fmt,
                                 ...) LLBC_STRING_FORMAT_CHECK(3, 4);;

#define LLBC_Print(fmt, ...) \
    LLBC_NS __LLBC_FilePrint(false, stdout, fmt, ##__VA_ARGS__)
#define LLBC_PrintLn(fmt, ...) \
    LLBC_NS __LLBC_FilePrint(true, stdout, fmt, ##__VA_ARGS__)
#define LLBC_FilePrint(file, fmt, ...) \
    LLBC_NS __LLBC_FilePrint(false, file, fmt, ##__VA_ARGS__)
#define LLBC_FilePrintLn(file, fmt, ...) \
    LLBC_NS __LLBC_FilePrint(true, file, fmt, ##__VA_ARGS__)

/**
 * Flush specific file buffer.
 * @param[in] file - file pointer.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXPORT int LLBC_FlushFile(FILE *file);

__LLBC_NS_END
