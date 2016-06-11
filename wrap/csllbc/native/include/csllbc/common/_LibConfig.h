/**
 * @file    _LibConfig.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/20
 * @version 1.0
 *
 * @brief   All csllbc_native, llbc core library configs will export in this file(VIA functions).
 */
#ifndef __CSLLBC_COM__LIB_CONFIG_H__
#define __CSLLBC_COM__LIB_CONFIG_H__

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
// LLBC_CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_IsDeafaultEnabledColourfulOutput();
// LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_IsDefaultLogToFile();
// LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultLogFileName(sint64 name, int nameLen);
// LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_DefaultFileLogPattern(sint64 pattern, int patternLen);
// LLBC_CFG_LOG_DEFAULT_DAILY_MODE
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Log_IsDefaultEnabledDailyMode();
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
// LLBC_CFG_COMM_USE_FULL_STACK
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_LibConfig_Comm_IsUsedFullStack();
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

#endif // !__CSLLBC_COM__LIB_CONFIG_H__
