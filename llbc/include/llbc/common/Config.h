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

#ifndef __LLBC_COM_CONFIG_H__
#define __LLBC_COM_CONFIG_H__

#include "llbc/common/PFConfig.h"

/**
 * \brief OS about config options define.
 */
// Determine library implement gettimeofday() API or not, WIN32 specific.
#define LLBC_CFG_OS_IMPL_GETTIMEOFDAY                       0
// Default listen backlog size.
#if LLBC_TARGET_PLATFORM_WIN32
 #define LLBC_CFG_OS_DFT_BACKLOG_SIZE                       (SOMAXCONN)
#else
 #define LLBC_CFG_OS_DFT_BACKLOG_SIZE                       (SOMAXCONN)
#endif
// Determine implement symbol about functions or not.
#define LLBC_CFG_OS_IMPL_SYMBOL                             1
// Determine max symbol name when enabled OS/Symbol functions.
#define LLBC_CFG_OS_SYMBOL_MAX_SYMBOL_NAME                  63
// Determine max cpature frames count when enabled OS/Symbol functions.
#define LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES               100

/**
 * \brief Core/File about config options define.
 */
// Define the LLBC_File class CopyFile method copy buffer size, in bytes, default is 16MB.
// Only used in Non-WIN32 systems.
#define LLBC_CFG_CORE_FILE_COPY_BUF_SIZE                    16384000

/**
 * \brief Core/Utils about config options define.
 */
// Determine library impl _itoa() API or not, Non-WIN32 Platform specific.
#define LLBC_CFG_CORE_UTILS_IMPL__ITOA                      0
// Determine library impl _i64toa() API or not, Non-WIN32 Platform specific.
#define LLBC_CFG_CORE_UTILS_IMPL__I64TOA                    0
// Determine library impl _ui64toa() API or not, Non-WIN32 Platform specific.
#define LLBC_CFG_CORE_UTILS_IMPL__UI64TOA                   0

/**
 * \brief Core/Sampler about config options define.
 */
// The interval sampler sampling hours value, default is 1.
#define LLBC_CFG_CORE_SAMPLER_INTERVAL_SAMPLING_HOURS       1

/**
 * \brief Core/Thread about config options define.
 */
// Enable/Disable Suspend/Resume thread support macro(Non-WIN32 platform available only).
// If enable it, LLBC library will use SIGUSR1, SIGUSR2 signal to implement.
#define LLBC_CFG_THREAD_ENABLE_SUSPEND_RESUME_SUPPORT       1
// Maximum thread number.
#define LLBC_CFG_THREAD_MAX_THREAD_NUM                      128
// Default stack size.
#define LLBC_CFG_THREAD_DFT_STACK_SIZE                      (10 * 1024 * 1024)
// Minimum stack size.
#define LLBC_CFG_THREAD_MINIMUM_STACK_SIZE                  (1 * 1024 * 1024)
// Message block default size.
#define LLBC_CFG_THREAD_MSG_BLOCK_DFT_SIZE                  (64)
// If you want debug guardians, enable this config option.
#define LLBC_CFG_THREAD_GUARD_DEBUG                         0

/**
 * \brief Core/Log about config options define.
 */
// Default log level is set to DEBUG.
#define LLBC_CFG_LOG_DEFAULT_LEVEL                          0
// Default DEBUG/INFO level log to console flush attr.
# define LLBC_CFG_LOG_DIRECT_FLUSH_TO_CONSOLE               1
// Default log asynchronous mode is set to false.
#define LLBC_CFG_LOG_DEFAULT_ASYNC_MODE                     0
// Default is log to console.
#define LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE                 1
// Default console log pattern: time [Logger Name][Log Level] - Message\n.
#define LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN            "%T [%N][%L] - %m%n"
// Default console colourful output enabled flag.
# define LLBC_CFG_LOG_DEFAULT_ENABLED_COLOURFUL_OUTPUT      1
// Default is not log to file.
#define LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE                    0
// Default log file name.
#define LLBC_CFG_LOG_DEFAULT_LOG_FILE_NAME                  "llbc.log"
// Default log file suffix.
#define LLBC_CFG_LOG_DEFAULT_LOG_FILE_SUFFIX                ""
// Default force application log path
#define LLBC_CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH             false
// Default log code file path control config
#define LLBC_CFG_LOG_DEFAULT_LOG_CODE_FILE_PATH             true
// Default file log pattern: time file:line@[Logger Name][Log Level] - Message\n.
#define LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN               "%T %f:%l@[%N][%L] - %m%n"
// Default is daily rolling mode.
#define LLBC_CFG_LOG_DEFAULT_DAILY_MODE                     1
// Default max log file size.
#define LLBC_CFG_LOG_MAX_FILE_SIZE                          (LONG_MAX)
// Default max backup file index.
#define LLBC_CFG_LOG_MAX_BACKUP_INDEX                       1000
// Default log file buffer size, in bytes.
#define LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE           1024000
// Default log appenders flush interval, in milli-seconds.
#define LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL             200
// Default max log appenders flush interval, in milli-seconds.
#define LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL                 1000
// Default log using mode.
#define LLBC_CFG_LOG_USING_WITH_STREAM                      0
// Default take over config, only using in root logger, when a message log to 
// unconfiged logger, root logger will take over, if configed takeover to true.
#define LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED       1
// Default logfile create option
#define LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE                   0

/**
 * \brief core/timer about configs.
 */
// strict timer schedule mode.
#define LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE                 0
// Long timeout time, when a timer timeout time >= <this value>, when call Cancel(), will force remove from binary heap.
#define LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME               864000000 // 10 days

/**
* \brief core/objectpool about configs.
*/
// object pool memory block size
#define LLBC_CFG_CORE_OBJECT_POOL_MEMORY_BLOCK_SIZE         40960
// object reset metch methods control.
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_clear      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Clear      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reset      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reset      1

/**
 * \brief ObjBase about configs.
 */
// Dictionary default bucket size.
#define LLBC_CFG_OBJBASE_DICT_DFT_BUCKET_SIZE               100
// Dictionary string key hash algorithm(case insensitive).
// Supports: SDBM, RS, JS, PJW, ELF, BKDR, DJB, AP
// Default: BKDR
#define LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO                 "BKDR"

/**
 * \brief Communication about configs.
 */
// The network data order define, default is non-net order.
#define LLBC_CFG_COMM_ORDER_IS_NET_ORDER                    1
// Default connect timeout time.
#define LLBC_CFG_COMM_DFT_CONN_TIMEOUT                      10
// The network concurrent listen sockets count.
#define LLBC_CFG_COMM_MAX_EVENT_COUNT                       100
// The epool max listen socket fd size(LINUX platform specific, only available before 2.6.8 version kernel before).
#define LLBC_CFG_EPOLL_MAX_LISTEN_FD_SIZE                   10000
// Default socket send buffer size.
#define LLBC_CFG_COMM_DFT_SEND_BUF_SIZE                     65536
// Default socket recv buffer size.
#define LLBC_CFG_COMM_DFT_RECV_BUF_SIZE                     65536
// Default service FPS value.
#define LLBC_CFG_COMM_DFT_SERVICE_FPS                       60
// Min service FPS value.
#define LLBC_CFG_COMM_MIN_SERVICE_FPS                       1
// Max service FPS value.
#define LLBC_CFG_COMM_MAX_SERVICE_FPS                       2000
// Sampler support option, default is true.
#define LLBC_CFG_COMM_ENABLE_SAMPLER_SUPPORT                1
// Per thread drive max services count.
#define LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT        16
// Determine full stack attribute.
#define LLBC_CFG_COMM_USE_FULL_STACK                        0
// Determine enable the service has status handler support or not.
#define LLBC_CFG_COMM_ENABLE_STATUS_HANDLER                 1
// Determine enable the service has status desc support or not.
#define LLBC_CFG_COMM_ENABLE_STATUS_DESC                    1
// Determine enable the unify pre-subscribe handler support or not.
#define LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE             1

// The poller model config(Platform specific).
//  Alloc set one of the follow configs(string format, case insensitive).
//   "SelectPoller" : Use select poller(All platform available).
//   "EpollPoller"  : Epoll poller(Avaliable in LINUX/Android platform).
//   "IocpPoller"   : Iocp poller(Available in WIN32 platform).
#if LLBC_TARGET_PLATFORM_LINUX
 #define LLBC_CFG_COMM_POLLER_MODEL                 "EpollPoller"
#elif LLBC_TARGET_PLATFORM_WIN32
 #define LLBC_CFG_COMM_POLLER_MODEL                 "IocpPoller"
#elif LLBC_TARGET_PLATFORM_IPHONE
 #define LLBC_CFG_COMM_POLLER_MODEL                 "SelectPoller"
#elif LLBC_TARGET_PLATFORM_MAC
 #define LLBC_CFG_COMM_POLLER_MODEL                 "SelectPoller"
#else
 #define LLBC_CFG_COMM_POLLER_MODEL                 "SelectPoller"
#endif

/**
 * \brief Application about configs.
 */
// Minidump types define, library default use full opts to write minidump file, you also can use <MiniDumpNormal> opt to replace it if you don' need write large & full minidump file.
// About Minidump type, see: https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/ne-minidumpapiset-minidump_type
#if LLBC_TARGET_PLATFORM_WIN32
 #define LLBC_CFG_APP_DUMPFILE_DUMPTYPES            (MiniDumpWithFullMemoryInfo | \
                                                     MiniDumpIgnoreInaccessibleMemory | \
                                                     MiniDumpWithFullAuxiliaryState | \
                                                     MiniDumpWithHandleData | \
                                                     MiniDumpWithIndirectlyReferencedMemory)
#endif

#endif // !__LLBC_COM_CONFIG_H__
