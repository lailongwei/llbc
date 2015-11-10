/**
 * @file    Version.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/18
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/common/ThirdHeader.h"
#include "llbc/common/Config.h"
#include "llbc/common/Compiler.h"
#include "llbc/common/Version.h"

__LLBC_NS_BEGIN

int LLBC_majorVersion       = 1;
int LLBC_minorVersion       = 0;
int LLBC_updateNo           = 2;

#ifdef LLBC_DEBUG
bool LLBC_isDebugVer        = true;
#else
bool LLBC_isDebugVer        = false;
#endif

LLBC_String LLBC_GetVersionInfo(bool verbose)
{
    LLBC_String desc;
    desc.format("%d.%d.%d_%s", LLBC_majorVersion, 
        LLBC_minorVersion, LLBC_updateNo, LLBC_isDebugVer ? "debug" : "release");

#if LLBC_TARGET_PLATFORM_WIN32
    desc.append_format("(%s", "WIN32");
#elif LLBC_TARGET_PLATFORM_LINUX
    desc.append_format("(%s", "LINUX");
#elif LLBC_TARGET_PLATFORM_IPHONE
    desc.append_format("(%s", "IPHONE");
#elif LLBC_TARGET_PLATFORM_MAC
    desc.append_format("(%s", "MAC");
#elif LLBC_TARGET_PLATFORM_ANDROID
    desc.append_format("(%s", "ANDROID");
#endif
    desc.append_format(", compiled with: %s)", LLBC_CUR_COMP_DESC);

    if (!verbose)
        return desc;

    // Append core info.
    desc.append_format("\n");
    desc.append_format("core info: \n");
    desc.append_format("  thread info: \n");
    desc.append_format("    max thread num: %d\n", LLBC_CFG_THREAD_MAX_THREAD_NUM);
    desc.append_format("    default thread stack size: %d\n", LLBC_CFG_THREAD_DFT_STACK_SIZE);
    desc.append_format("    miximum thread stack size: %d\n", LLBC_CFG_THREAD_MINIMUM_STACK_SIZE);
    desc.append_format("    message block size: %d\n", LLBC_CFG_THREAD_MSG_BLOCK_DFT_SIZE);
    desc.append_format("  logger info: \n");
    desc.append_format("    default level: %d\n", LLBC_CFG_LOG_DEFAULT_LEVEL);
    desc.append_format("    default asynchronous mode: %s\n", (LLBC_CFG_LOG_DEFAULT_ASYNC_MODE) ? "true" : "false");
    desc.append_format("    default log to console: %s\n", (LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE) ? "true" : "false");
    desc.append_format("    default console log pattern: %s\n", LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN);
    desc.append_format("    default log to file: %s\n", (LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE) ? "true" : "false");
    desc.append_format("    default log file name: %s\n", LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME);
    desc.append_format("    default file log pattern: %s\n", LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN);
    desc.append_format("    default daily mode enabled(available in file log): %s\n", (LLBC_CFG_LOG_DEFAULT_DAILY_MODE) ? "true" : "false");
    desc.append_format("    default max log file size: %d\n", LLBC_CFG_LOG_DEFAULT_MAX_FILE_SIZE);
    desc.append_format("    default max backup index: %d\n", LLBC_CFG_LOG_DEFAULT_MAX_BACKUP_INDEX);

    // Append communication info.
    desc.append_format("communication info: \n");
    desc.append_format("  poller model: %s\n", LLBC_CFG_COMM_POLLER_MODEL);
    desc.append_format("  default service FPS: %d\n", LLBC_CFG_COMM_DFT_SERVICE_FPS);
    desc.append_format("  per thread max drive services count: %d\n", LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT);
    desc.append_format("  full protocol-stack support(let your program more efficient): %s\n", LLBC_CFG_COMM_USE_FULL_STACK ? "true" : "false");
    desc.append_format("  enabled register status code handler support: %s\n", LLBC_CFG_COMM_ENABLE_STATUS_HANDLER ? "true" : "false");
    desc.append_format("  enabled register status code describe string support: %s\n", LLBC_CFG_COMM_ENABLE_STATUS_DESC ? "true" : "false");
    desc.append_format("  enabled unify pre-subscribe support: %s\n", LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE ? "true" : "false");
    desc.append_format("  sampler support: %s\n", LLBC_CFG_COMM_ENABLE_SAMPLER_SUPPORT ? "true" : "false");

    // Append third-party info.
    desc.append_format("3rd party info: \n");
    desc.append_format("  zlib version: %s(ver-num:%d)\n", ZLIB_VERSION, ZLIB_VERNUM);

    return desc;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
