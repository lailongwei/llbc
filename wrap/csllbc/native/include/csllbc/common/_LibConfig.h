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

#include "csllbc/common/LibHeader.h"
#include "csllbc/common/Macro.h"

///////////////////////////////////////////////////////////////////////////////////
// llbc core library configs export function definitions.
//
// Common about configs.
//
// LLBC_CFG_OS_DFT_BACKLOG_SIZE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_DefaultBacklogSize();

//
// Log about configs.
//
// LLBC_CFG_LOG_ROOT_LOGGER_NAME
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_RootLoggerName(sint64 name, int nameLen);
// LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultNotConfigOptionUse(sint64 notConfigUse, int len);
// LLBC_CFG_LOG_DEFAULT_LEVEL
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultLogLevel();
// LLBC_CFG_LOG_DEFAULT_FLUSH_TO_CONSOLE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_IsDirectFlushToConsole();
// LLBC_CFG_LOG_DEFAULT_ASYNC_MODE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultIsAsyncMode();
// LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultLogToConsole();
// LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultConsoleLogPattern(sint64 pattern, int patternLen);
// LLBC_CFG_LOG_DEFAULT_COLOURFUL_OUTPUT
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_IsDefaultColourfulOutput();
// LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_IsDefaultLogToFile();
// LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultFileLogPattern(sint64 pattern, int patternLen);
// LLBC_CFG_LOG_DEFAULT_DAILY_ROLLING
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultFileRollingMode();
// LLBC_CFG_LOG_MAX_FILE_SIZE
LLBC_EXTERN_C CSLLBC_EXPORT sint64 csllbc_LibConfig_Log_MaxFileSize();
// LLBC_CFG_LOG_MAX_BACKUP_INDEX
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_MaxBackupIndex();
// LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultLogFileBufferSize();
// LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultLogFlushInterval();
// LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_MaxLogFlushInterval();
// LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_TakeOverUnknownLoggerMsg();
// LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_LazyCreateLogFile();

// Thread about configs.
//
// LLBC_CFG_THREAD_GUARD_DEBUG
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Thread_GuardDebug();

//
// Timer about configs.
//
// LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Timer_StrictSchedule();

//
// Network about configs.
//
// LLBC_CFG_COMM_ORDER_IS_NET_ORDER
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_IsNetOrder();
// LLBC_CFG_COMM_DFT_CONN_TIMEOUT
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_DefaultConnTimeout();
// LLBC_CFG_COMM_MAX_EVENT_COUNT
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_MaxEventCount();
// LLBC_CFG_COMM_DFT_SERVICE_FPS
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_DefaultServiceFPS();
// LLBC_CFG_COMM_MIN_SERVICE_FPS
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_MinServiceFPS();
// LLBC_CFG_COMM_MAX_SERVICE_FPS
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_MaxServiceFPS();
// LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_PerThreadDriveMaxServiceCount();
// LLBC_CFG_COMM_ENABLE_SAMPLER_SUPPORT
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_IsEnabledSamplerSupport();
// LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_IsEnabledStatusHandler();
// LLBC_CFG_COMM_ENABLE_STATUS_DESC
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_IsEnabledStatusDesc();
// LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_IsEnabledUnifyPreSubscribe();
// LLBC_CFG_COMM_POLLER_MODEL
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_PollerModel(sint64 model, int modelLen);
//
///////////////////////////////////////////////////////////////////////////////////


