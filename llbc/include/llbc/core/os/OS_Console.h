/**
 * @file    OS_Console.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_OS_OS_CONSOLE_H__
#define __LLBC_CORE_OS_OS_CONSOLE_H__

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
        Fg_Black,
        Fg_Red,
        Fg_Green,
        Fg_Blue,
        Fg_Yellow,
        Fg_Purple,
        Fg_Cyan,
        Fg_White,
        
        Bg_Black,
        Bg_Red,
        Bg_Green,
        Bg_Blue,
        Bg_Yellow,
        Bg_Purple,
        Bg_Cyan,
        Bg_White,

        Highlight_Fg,
        Highlight_Bg,
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
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

        Fg_Default = Fg_White,
        Bg_Default = Bg_Black
    };
};

/**
 * Get console color value.
 * @param[in] file - console type, can set to stdout or stderr.
 * @return int - console color, if return -1, it means error occurred.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_GetConsoleColor(FILE *file);

/**
 * Set console color value.
 * @param[in] file  - console type, can set to stdout or stderr.
 * @param[in] color - color value.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_SetConsoleColor(FILE *file, int color);

/**
 * Secure output message to console.
 * @param[in] newline - the new line flag, if specific, will auto append NewLine character(s).
 * @param[in] file    - console type, can set to stdout or stderr.
 * @param[in] fmt     - format control string.
 * @param[in] ...     - option arguments.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int __LLBC_FilePrint(bool newline, FILE *file, const char *fmt, ...);

#define LLBC_Print(fmt, ...) \
    LLBC_NS __LLBC_FilePrint(false, stdout, fmt, ##__VA_ARGS__)
#define LLBC_PrintLine(fmt, ...) \
    LLBC_NS __LLBC_FilePrint(true, stdout, fmt, ##__VA_ARGS__)
#define LLBC_FilePrint(file, fmt, ...) \
    LLBC_NS __LLBC_FilePrint(false, file, fmt, ##__VA_ARGS__)
#define LLBC_FilePrintLine(file, fmt, ...) \
    LLBC_NS __LLBC_FilePrint(true, file, fmt, ##__VA_ARGS__)

/**
 * Flush specific file buffer.
 * @param[in] file - file pointer.
 * @return int - return 0 if success, otherwise return -1.
 */
LLBC_EXTERN LLBC_EXPORT int LLBC_FlushFile(FILE *file);

__LLBC_NS_END

#endif // !__LLBC_CORE_OS_OS_CONSOLE_H__
