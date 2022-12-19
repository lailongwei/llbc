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

#include "csllbc/common/_LibConfig.h"

LLBC_BEGIN_C_DECL

int csllbc_LibConfig_DefaultBacklogSize()
{
    return LLBC_CFG_OS_DFT_BACKLOG_SIZE;
}

int csllbc_LibConfig_Log_RootLoggerName(sint64 name, int nameLen)
{
    const size_t writable = MIN(strlen(LLBC_CFG_LOG_ROOT_LOGGER_NAME), static_cast<size_t>(nameLen));
    memcpy(reinterpret_cast<char *>(name), LLBC_CFG_LOG_ROOT_LOGGER_NAME, writable);

    return static_cast<int>(writable);
}

int csllbc_LibConfig_Log_DefaultNotConfigOptionUse(sint64 notConfigUse, int len)
{
    const size_t writable = MIN(strlen(LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE), static_cast<size_t>(len));
    memcpy(reinterpret_cast<char *>(notConfigUse), LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE, writable);

    return static_cast<int>(writable);
}

int csllbc_LibConfig_Log_DefaultLogLevel()
{
    return LLBC_CFG_LOG_DEFAULT_LEVEL;
}

int csllbc_LibConfig_Log_IsDirectFlushToConsole()
{
    return LLBC_CFG_LOG_DIRECT_FLUSH_TO_CONSOLE;
}

int csllbc_LibConfig_Log_DefaultIsAsyncMode()
{
    return LLBC_CFG_LOG_DEFAULT_ASYNC_MODE;
}

int csllbc_LibConfig_Log_DefaultLogToConsole()
{
    return LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE;
}

int csllbc_LibConfig_Log_DefaultConsoleLogPattern(sint64 pattern, int patternLen)
{
    const int writable = MIN(static_cast<int>(strlen(LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN)), patternLen);
    memcpy(reinterpret_cast<char *>(pattern), LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN, writable);

    return writable;
}

int csllbc_LibConfig_Log_IsDefaultColourfulOutput()
{
    return LLBC_CFG_LOG_DEFAULT_COLOURFUL_OUTPUT;
}

int csllbc_LibConfig_Log_IsDefaultLogToFile()
{
    return LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE;
}

int csllbc_LibConfig_Log_DefaultFileLogPattern(sint64 pattern, int patternLen)
{
    const int writable = MIN(static_cast<int>(strlen(LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN)), patternLen);
    memcpy(reinterpret_cast<char *>(pattern), LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN, writable);

    return writable;
}

int csllbc_LibConfig_Log_DefaultFileRollingMode()
{
    return LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE;
}

sint64 csllbc_LibConfig_Log_MaxFileSize()
{
    return LLBC_CFG_LOG_MAX_FILE_SIZE;
}

int csllbc_LibConfig_Log_MaxBackupIndex()
{
    return LLBC_CFG_LOG_MAX_BACKUP_INDEX;
}

int csllbc_LibConfig_Log_DefaultLogFileBufferSize()
{
    return LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE;
}

int csllbc_LibConfig_Log_DefaultLogFlushInterval()
{
    return LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL;
}

int csllbc_LibConfig_Log_MaxLogFlushInterval()
{
    return LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL;
}

int csllbc_LibConfig_Log_TakeOverUnknownLoggerMsg()
{
    return LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED;
}

int csllbc_LibConfig_Log_LazyCreateLogFile()
{
    return LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE;
}

int csllbc_LibConfig_Thread_GuardDebug()
{
    return LLBC_CFG_THREAD_GUARD_DEBUG;
}

int csllbc_LibConfig_Timer_StrictSchedule()
{
    return LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE;
}

int csllbc_LibConfig_Comm_IsNetOrder()
{
    return LLBC_CFG_COMM_ORDER_IS_NET_ORDER;
}

int csllbc_LibConfig_Comm_DefaultConnTimeout()
{
    return LLBC_CFG_COMM_DFT_CONN_TIMEOUT;
}

int csllbc_LibConfig_Comm_MaxEventCount()
{
    return LLBC_CFG_COMM_MAX_EVENT_COUNT;
}

int csllbc_LibConfig_Comm_DefaultServiceFPS()
{
    return LLBC_CFG_COMM_DFT_SERVICE_FPS;
}

int csllbc_LibConfig_Comm_MinServiceFPS()
{
    return LLBC_CFG_COMM_MIN_SERVICE_FPS;
}

int csllbc_LibConfig_Comm_MaxServiceFPS()
{
    return LLBC_CFG_COMM_MAX_SERVICE_FPS;
}

int csllbc_LibConfig_Comm_IsEnabledSamplerSupport()
{
    return LLBC_CFG_COMM_ENABLE_SAMPLER_SUPPORT;
}

int csllbc_LibConfig_Comm_PerThreadDriveMaxServiceCount()
{
    return LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT;
}

int csllbc_LibConfig_Comm_IsEnabledStatusHandler()
{
    return LLBC_CFG_COMM_ENABLE_STATUS_HANDLER;
}

int csllbc_LibConfig_Comm_IsEnabledStatusDesc()
{
    return LLBC_CFG_COMM_ENABLE_STATUS_DESC;
}

int csllbc_LibConfig_Comm_IsEnabledUnifyPreSubscribe()
{
    return LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE;
}

int csllbc_LibConfig_Comm_PollerModel(sint64 model, int modelLen)
{
    const int writable = MIN(static_cast<int>(strlen(LLBC_CFG_COMM_POLLER_MODEL)), modelLen);
    memcpy(reinterpret_cast<char *>(model), LLBC_CFG_COMM_POLLER_MODEL, writable);

    return writable;
}

LLBC_END_C_DECL