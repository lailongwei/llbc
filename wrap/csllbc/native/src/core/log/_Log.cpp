/**
 * @file    _Log.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/07/09
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc/core/log/_Log.h"

LLBC_BEGIN_C_DECL

int csllbc_Log_Init(const char *cfgFile)
{
    return LLBC_LoggerManagerSingleton->Initialize(cfgFile);
}

int csllbc_Log_IsInited()
{
    return LLBC_LoggerManagerSingleton->IsInited() ? 1 : 0;
}

LLBC_Logger *csllbc_Log_GetLogger(const char *loggerName)
{
    return LLBC_LoggerManagerSingleton->GetLogger(loggerName);
}

int csllbc_Log_GetLogLevel(LLBC_Logger *logger)
{
    return logger->GetLogLevel();
}

int csllbc_Log_LogMsg(LLBC_Logger *logger,
                      const char *fileName,
                      int lineNo,
                      int level,
                      const char *msg,
                      const char *tag)
{
    return logger->Output(level, tag, fileName, lineNo, "%s", msg); 
}

void csllbc_Log_Destroy()
{
    LLBC_LoggerManagerSingleton->Finalize();
}

LLBC_END_C_DECL