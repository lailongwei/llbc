/**
 * @file    _Log.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/07/09
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_CORE_LOG__LOG_H__
#define __CSLLBC_CORE_LOG__LOG_H__

#include "csllbc/common/Common.h"

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Log_Init(const char *cfgFile);

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Log_IsInited();

LLBC_EXTERN_C CSLLBC_EXPORT LLBC_Logger *csllbc_Log_GetLogger(const char *loggerName);

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Log_GetLogLevel(LLBC_Logger *logger);

LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Log_LogMsg(LLBC_Logger *logger,
                                                  const char *fileName,
                                                  int lineNo,
                                                  int level,
                                                  const char *msg,
                                                  const char *tag);

LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Log_Destroy();

#endif // !__CSLLBC_CORE_LOG__LOG_H__
