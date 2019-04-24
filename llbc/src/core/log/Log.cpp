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

#include "llbc/core/os/OS_Console.h"
#include "llbc/core/thread/Guard.h"
#include "llbc/core/utils/Util_Debug.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/Logger.h"
#include "llbc/core/log/LoggerManager.h"

#include "llbc/core/json/json.h"

#include "llbc/core/log/Log.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_LogHelper __llbc_logHelper;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_LogHelper &__LLBC_GetLogHelper()
{
    return LLBC_INTERNAL_NS __llbc_logHelper;
}

LLBC_Logger *LLBC_LogHelper::_rootLogger = NULL;
LLBC_LoggerManager *LLBC_LogHelper::_loggerManager = NULL;

#define __LLBC_LOG_TO_ROOT(level, fmt)                                        \
    char *fmttedMsg; int msgLen;                                              \
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);                                   \
                                                                              \
    LLBC_FreeGuard<char> guard(fmttedMsg);                                    \
                                                                              \
    if (LIKELY(_rootLogger))                                                  \
        _rootLogger->OutputNonFormat(level, NULL, __FILE__, __LINE__, fmttedMsg, msgLen); \
    else                                                                      \
        UnInitOutput(level >= _LV::Warn ? stderr : stdout, fmttedMsg);        \

#define __LLBC_LOG_TO_SPEC(logger, level, tag, fmt)                           \
    char *fmttedMsg; int msgLen;                                              \
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);                                   \
                                                                              \
    LLBC_FreeGuard<char> guard(fmttedMsg);                                    \
                                                                              \
    LLBC_Logger *l = NULL;                                                    \
    if (logger == NULL)                                                       \
        l = _rootLogger;                                                      \
    else if (LIKELY(_loggerManager))                                          \
        l = _loggerManager->GetLogger(logger);                                \
                                                                              \
    if (LIKELY(l))                                                            \
        l->OutputNonFormat(level, tag, __FILE__, __LINE__, fmttedMsg, msgLen);\
    else                                                                      \
        UnInitOutput(level >= _LV::Warn ? stderr : stdout, fmttedMsg);        \

#define __LLBC_JLOG_TO_SPEC(logger, level, tag, jMsg)                               \
                                                                                    \
    if (LIKELY(logger))                                                             \
        logger->OutputNonFormat(level, tag, __FILE__, __LINE__, jMsg, strlen(jMsg));\
    else                                                                            \
        LLBC_LogHelper::UnInitOutput(level >= _LV::Warn ? stderr : stdout, jMsg);   \

LLBC_LogJsonMsg::LLBC_LogJsonMsg(LLBC_Logger* logger, const char* tag, int lv):
    _logger(logger),
    _tag(tag),
    _lv(lv)
{
}

LLBC_LogJsonMsg::~LLBC_LogJsonMsg()
{
}

void LLBC_LogJsonMsg::Finish(LLBC_Logger* logger, int lv, const char* tag, const char *message)
{
    __LLBC_JLOG_TO_SPEC(logger, lv, tag, message);
}

int LLBC_LogHelper::init(const LLBC_String &cfgFile)
{
    return LLBC_LoggerManagerSingleton->Initialize(cfgFile);
}

void LLBC_LogHelper::destroy()
{
    LLBC_LoggerManagerSingleton->Finalize();
}

void LLBC_LogHelper::Initialize(LLBC_LoggerManager *loggerManager)
{
    if (UNLIKELY(_rootLogger))
    {
        trace("llbc library internal error, repeat to init Log");
        return;
    }

    _loggerManager = loggerManager;
    _rootLogger = _loggerManager->GetRootLogger();
}

void LLBC_LogHelper::Finalize()
{
    if (UNLIKELY(!_rootLogger))
    {
        trace("llbc library internal error, try to destroy uninit Log");
        return;
    }

    _loggerManager = NULL;
    _rootLogger = NULL;
}

void LLBC_LogHelper::d(const char *fmt, ...)
{
    __LLBC_LOG_TO_ROOT(_LV::Debug, fmt);
}

void LLBC_LogHelper::d2(const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(NULL, _LV::Debug, tag, fmt);
}

void LLBC_LogHelper::d3(const char *logger, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Debug, NULL, fmt);
}

void LLBC_LogHelper::d4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Debug, tag, fmt);
}

void LLBC_LogHelper::i(const char *fmt, ...)
{
    __LLBC_LOG_TO_ROOT(_LV::Info, fmt);
}

void LLBC_LogHelper::i2(const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(NULL, _LV::Info, tag, fmt);
}

void LLBC_LogHelper::i3(const char *logger, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Info, NULL, fmt);
}

void LLBC_LogHelper::i4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Info, tag, fmt);
}

void LLBC_LogHelper::w(const char *fmt, ...)
{
    __LLBC_LOG_TO_ROOT(_LV::Warn, fmt);
}

void LLBC_LogHelper::w2(const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(NULL, _LV::Warn, tag, fmt);
}

void LLBC_LogHelper::w3(const char *logger, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Warn, NULL, fmt);
}

void LLBC_LogHelper::w4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Warn, tag, fmt);
}

void LLBC_LogHelper::e(const char *fmt, ...)
{
    __LLBC_LOG_TO_ROOT(_LV::Error, fmt);
}

void LLBC_LogHelper::e2(const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(NULL, _LV::Error, tag, fmt);
}

void LLBC_LogHelper::e3(const char *logger, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Error, NULL, fmt);
}

void LLBC_LogHelper::e4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Error, tag, fmt);
}

void LLBC_LogHelper::f(const char *fmt, ...)
{
    __LLBC_LOG_TO_ROOT(_LV::Fatal, fmt);
}

void LLBC_LogHelper::f2(const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(NULL, _LV::Fatal, tag, fmt);
}

void LLBC_LogHelper::f3(const char *logger, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Fatal, NULL, fmt);
}

void LLBC_LogHelper::f4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_LOG_TO_SPEC(logger, _LV::Fatal, tag, fmt);
}

void LLBC_LogHelper::UnInitOutput(FILE *to, const char *msg)
{
    LLBC_FilePrint(to, "[Log] %s\n", msg);
    if (to != stderr)
        LLBC_FlushFile(to);
}

//! At latest, undef code define macros.
#undef __LLBC_LOG_TO_ROOT
#undef __LLBC_LOG_TO_SPEC

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
