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

#include "csllbc/core/log/_Log.h"

LLBC_BEGIN_C_DECL

int csllbc_Log_Init(const char *cfgFile)
{
    return LLBC_LoggerMgrSingleton->Initialize(cfgFile);
}

int csllbc_Log_IsInited()
{
    return LLBC_LoggerMgrSingleton->IsInited() ? 1 : 0;
}

LLBC_Logger *csllbc_Log_GetLogger(const char *loggerName)
{
    return LLBC_LoggerMgrSingleton->GetLogger(loggerName);
}

int csllbc_Log_GetLogLevel(LLBC_Logger *logger)
{
    return logger->GetLogLevel();
}

int csllbc_Log_LogMsg(LLBC_Logger *logger,
                      const char *fileName,
                      int lineNo,
                      const char *func,
                      int level,
                      const char *msg,
                      const char *tag)
{
    return logger->NonFormatOutput(level,
                                   tag,
                                   fileName,
                                   lineNo,
                                   func,
                                   0,
                                   msg,
                                   strlen(msg)); 
}

void csllbc_Log_Destroy()
{
    LLBC_LoggerMgrSingleton->Finalize();
}

LLBC_END_C_DECL