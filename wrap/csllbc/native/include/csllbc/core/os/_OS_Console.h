/**
 * @file    _OS_Console.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/22
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_CORE_OS__OS_CONSOLE_H__
#define __CSLLBC_CORE_OS__OS_CONSOLE_H__

#include "csllbc/common/Common.h"

LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Console_Trace(const char *value, int valueLen);

LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Console_SafePrint(bool toStdout, bool newLine, const char *value, int valueLen);

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Console_GetForegroundColor();
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Console_SetForegroundColor(int color);

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Console_GetBackgroundColor();
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Console_SetBackgroundColor(int color);

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Console_ResetColor();

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Console_SafeFlush(bool flushStdout);

#endif // !__CSLLBC_CORE_OS__OS_CONSOLE_H__
