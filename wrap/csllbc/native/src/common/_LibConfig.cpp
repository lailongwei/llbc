/**
 * @file    _LibConfig.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/21
 * @version 1.0
 *
 * @brief   All csllbc_native, llbc core library configs will export in this file(VIA functions).
 */

#include "csllbc/common/Export.h"

#include "csllbc/common/_LibConfig.h"

LLBC_BEGIN_C_DECL

int csllbc_LibConfig_DefaultBacklogSize()
{
    return LLBC_CFG_OS_DFT_BACKLOG_SIZE;
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
    const int writable = MIN(LLBC_StrLenA(LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN), patternLen);
    LLBC_MemCpy(reinterpret_cast<char *>(pattern), LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN, writable);

    return writable;
}

int csllbc_LibConfig_Log_IsDeafaultEnabledColourfulOutput()
{
    return LLBC_CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT;
}

int csllbc_LibConfig_Log_IsDefaultLogToFile()
{
    return LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE;
}

int csllbc_LibConfig_Log_DefaultLogFileName(sint64 name, int nameLen)
{
    const int writable = MIN(LLBC_StrLenA(LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME), nameLen);
    LLBC_MemCpy(reinterpret_cast<char *>(name), LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME, writable);

    return writable;
}

int csllbc_LibConfig_Log_DefaultFileLogPattern(sint64 pattern, int patternLen)
{
    const int writable = MIN(LLBC_StrLenA(LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN), patternLen);
    LLBC_MemCpy(reinterpret_cast<char *>(pattern), LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN, writable);

    return writable;
}

int csllbc_LibConfig_Log_IsDefaultEnabledDailyMode()
{
    return LLBC_CFG_LOG_DEFAULT_DAILY_MODE;
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

int csllbc_LibConfig_Comm_IsUsedFullStack()
{
    return LLBC_CFG_COMM_USE_FULL_STACK;
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
    const int writable = MIN(LLBC_StrLenA(LLBC_CFG_COMM_POLLER_MODEL), modelLen);
    LLBC_MemCpy(reinterpret_cast<char *>(model), LLBC_CFG_COMM_POLLER_MODEL, writable);

    return writable;
}

LLBC_END_C_DECL