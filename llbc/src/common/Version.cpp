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

#include "llbc/common/ThirdHeader.h"
#include "llbc/common/Config.h"
#include "llbc/common/Compiler.h"
#include "llbc/common/Version.h"

__LLBC_NS_BEGIN

int LLBC_majorVersion       = 1;
int LLBC_minorVersion       = 1;
int LLBC_updateNo           = 0;

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
    desc.append_format("(%s", "WINDOWS");
#elif LLBC_TARGET_PLATFORM_LINUX
    desc.append_format("(%s", "LINUX");
#elif LLBC_TARGET_PLATFORM_IPHONE
    desc.append_format("(%s", "IPHONE");
#elif LLBC_TARGET_PLATFORM_MAC
    desc.append_format("(%s", "MAC");
#elif LLBC_TARGET_PLATFORM_ANDROID
    desc.append_format("(%s", "ANDROID");
#endif

#if LLBC_CUR_COMP == LLBC_COMP_MSVC
    desc.append_format(", arch:%s, compiled with:%s(ver:%d))", 
        LLBC_TARGET_PROCESSOR_DESC, LLBC_CUR_COMP_DESC, LLBC_COMP_VER);
#elif LLBC_CUR_COMP == LLBC_COMP_GCC
    desc.append_format(", arch:%s, compiled with:%s(ver:%d.%d.%d))", 
        LLBC_TARGET_PROCESSOR_DESC, LLBC_CUR_COMP_DESC, LLBC_COMP_MAJOR_VER, LLBC_COMP_MINOR_VER, LLBC_COMP_PATCH_LEVEL);
#else
    desc.append_format(", arch:%s, compiled with:%s)", 
        LLBC_TARGET_PROCESSOR_DESC, LLBC_CUR_COMP_DESC);
#endif

    if (!verbose)
        return desc;

    // Append core info.
    desc.append_format("\n");
    desc.append_format("core info: \n");
    desc.append_format("  os info: \n");
    desc.append_format("    enabled symbol support?: %s\n", LLBC_CFG_OS_IMPL_SYMBOL ? "true" : "false");
    desc.append_format("  thread info: \n");
    desc.append_format("    max thread num: %d\n", LLBC_CFG_THREAD_MAX_THREAD_NUM);
    desc.append_format("    default thread stack size: %d\n", LLBC_CFG_THREAD_DFT_STACK_SIZE);
    desc.append_format("    miximum thread stack size: %d\n", LLBC_CFG_THREAD_MINIMUM_STACK_SIZE);
    desc.append_format("    message block size: %d\n", LLBC_CFG_THREAD_MSG_BLOCK_DFT_SIZE);
    desc.append_format("    enabled guard debug: %s\n", LLBC_CFG_THREAD_GUARD_DEBUG ? "true" : "false");
    desc.append_format("  logger info: \n");
    desc.append_format("    root logger name: %s\n", LLBC_CFG_LOG_ROOT_LOGGER_NAME);
    desc.append_format("    default not config option use: %s\n", LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE);
    desc.append_format("    take over unknown logger message?: %s\n", LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED ? "true" : "false");
    desc.append_format("    default level: %d\n", LLBC_CFG_LOG_DEFAULT_LEVEL);
    desc.append_format("    default asynchronous mode: %s\n", (LLBC_CFG_LOG_DEFAULT_ASYNC_MODE) ? "true" : "false");
    desc.append_format("    default flush interval(ms)(only available in asyn mode): %d\n", LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL);
    desc.append_format("    max flush interval(ms)(only available in asyn mode): %d\n", LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL);
    desc.append_format("    default log to console: %s\n", (LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE) ? "true" : "false");
    desc.append_format("    default console log pattern: %s\n", LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN);
    desc.append_format("    default log to file: %s\n", (LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE) ? "true" : "false");
    desc.append_format("    default log file name: <same with logger name>\n");
    desc.append_format("    default file log pattern: %s\n", LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN);
    desc.append_format("    default file rolling mode: %d\n", LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE);
    desc.append_format("    default max log file size: %d\n", LLBC_CFG_LOG_MAX_FILE_SIZE);
    desc.append_format("    default max backup index: %d\n", LLBC_CFG_LOG_MAX_BACKUP_INDEX);
    desc.append_format("    default log file buffer size: %d\n", LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE);
    desc.append_format("    lazy create log file?: %s\n", LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE ? "true" : "false");
    desc.append_format("  timer info: \n");
    desc.append_format("    strict timer schedule: %s\n", LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE ? "true" : "false");
    desc.append_format("  objectpool info: \n");
    desc.append_format("    object pool per-block units number: %d\n", LLBC_CFG_CORE_OBJECT_POOL_BLOCK_UNITS_NUMBER);
    desc.append_format("    framework type[LLBC_Packet] per-block units number: %d\n", LLBC_CFG_CORE_OBJECT_POOL_PACKET_UNITS_NUMBER);
    desc.append_format("    framework type[LLBC_MessageBlock] per-block units number: %d\n", LLBC_CFG_CORE_OBJECT_POOL_MESSAGE_BLOCK_UNITS_NUMBER);

    // Append communication info.
    desc.append_format("communication info: \n");
    desc.append_format("  poller model: %s\n", LLBC_CFG_COMM_POLLER_MODEL);
    desc.append_format("  default service FPS: %d\n", LLBC_CFG_COMM_DFT_SERVICE_FPS);
    desc.append_format("  per thread max drive services count: %d\n", LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT);
    desc.append_format("  enabled register status code handler support: %s\n", LLBC_CFG_COMM_ENABLE_STATUS_HANDLER ? "true" : "false");
    desc.append_format("  enabled register status code describe string support: %s\n", LLBC_CFG_COMM_ENABLE_STATUS_DESC ? "true" : "false");
    desc.append_format("  enabled unify pre-subscribe support: %s\n", LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE ? "true" : "false");
    desc.append_format("  sampler support: %s\n", LLBC_CFG_COMM_ENABLE_SAMPLER_SUPPORT ? "true" : "false");

    // Append third-party info.
    // desc.append_format("3rd party info: \n");
    // desc.append_format("  zlib version: %s(ver-num:%d)\n", ZLIB_VERSION, ZLIB_VERNUM);

    return desc;
}

__LLBC_NS_END
