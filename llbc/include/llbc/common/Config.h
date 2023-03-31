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
#define LLBC_CFG_OS_SYMBOL_MAX_SYMBOL_NAME                  127
// Determine max cpature frames count when enabled OS/Symbol functions.
#define LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES               100
// Determine hook crash single set.
#define LLBC_CFG_OS_HOOK_CRASH_SINGLES                      {SIGSEGV, SIGABRT, SIGFPE}

/**
 * \brief Common about config options define.
 */
// The stream object auto resize increment limit, in bytes.
#define LLBC_CFG_COM_STREAM_AUTO_RESIZE_INCR_LIMIT          512

/**
 * \brief core/algo about config options define.
 */
// Define RingBuffer init capacity.
#define LLBC_CFG_CORE_ALGO_RING_BUFFER_DEFAULT_CAP          32

/**
 * \brief core/variant about config options define.
 */
// Define variant number as string method fast access begin number(included).
#define LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_BEGIN         ((int)-256)
// Define variant number as string method fast access end number(included).
#define LLBC_CFG_CORE_VARIANT_FAST_NUM_AS_STR_END           ((int)256)

/**
 * \brief core/file about config options define.
 */
// Define the LLBC_File class CopyFile method copy buffer size, in bytes, default is 16MB.
// Only used in Non-WIN32 systems.
#define LLBC_CFG_CORE_FILE_COPY_BUF_SIZE                    16384000

/**
 * \brief core/utils about config options define.
 */
// Determine library impl _itoa() API or not, Non-WIN32 Platform specific.
#define LLBC_CFG_CORE_UTILS_IMPL__ITOA                      0
// Determine library impl _i64toa() API or not, Non-WIN32 Platform specific.
#define LLBC_CFG_CORE_UTILS_IMPL__I64TOA                    0
// Determine library impl _ui64toa() API or not, Non-WIN32 Platform specific.
#define LLBC_CFG_CORE_UTILS_IMPL__UI64TOA                   0
// Determine define win32-styled string datatypes or not, eg:LPSTR/LPTSTR/...
#define LLBC_CFG_CORE_UTILS_DEF_WIN32_STYLED_STR_DATATYPES  0

/**
 * \brief core/sampler about config options define.
 */
// The interval sampler sampling hours value, default is 1.
#define LLBC_CFG_CORE_SAMPLER_INTERVAL_SAMPLING_HOURS       1

/**
 * \brief core/thread about config options define.
 */
// Maximum thread number.
#define LLBC_CFG_THREAD_MAX_THREAD_NUM                      128
// Default stack size.
#if LLBC_TARGET_PLATFORM_WIN32
// Stack size 0 operating system uses the same value as the stack that's specified for the main thread
#define LLBC_CFG_THREAD_DFT_STACK_SIZE                      0
#else
#define LLBC_CFG_THREAD_DFT_STACK_SIZE                      (10 * 1024 * 1024)
#endif
// Minimum stack size.
#define LLBC_CFG_THREAD_MINIMUM_STACK_SIZE                  (2 * 1024 * 1024)
// Message block default size.
#define LLBC_CFG_THREAD_MSG_BLOCK_DFT_SIZE                  (256)
// If you want debug guardians, enable this config option.
#define LLBC_CFG_THREAD_GUARD_DEBUG                         0
// Entry thread handle.
#define LLBC_CFG_THREAD_ENTRY_THREAD_HANDLE                 1
// Entry thread group handle.
#define LLBC_CFG_THREAD_ENTRY_THREAD_GROUP_HANDLE           1

/**
 * \brief core/log about config options define.
 */
// Root logger name.
#define LLBC_CFG_LOG_ROOT_LOGGER_NAME                       "root"
// Logger format buf size.
#define LLBC_CFG_LOG_FORMAT_BUF_SIZE                        16 * 1024
// Default log level is set to DEBUG(TRACE:0, DEBUG:1, INFO:2, WARN:3, ERROR:4, FATAL:5).
#define LLBC_CFG_LOG_DEFAULT_LEVEL                          0
// Default DEBUG/INFO level log to console flush attr.
# define LLBC_CFG_LOG_DIRECT_FLUSH_TO_CONSOLE               0
// Default log asynchronous mode is set to false.
#define LLBC_CFG_LOG_DEFAULT_ASYNC_MODE                     0
// Default log independent logger thread is set to false.
#define LLBC_CFG_LOG_DEFAULT_INDEPENDENT_THREAD             0
// Default add timestamp in json log is set to false.
#define LLBC_CFG_LOG_DEFAULT_ADD_TIMESTAMP_IN_JSON_LOG      0
// Default is log to console.
#define LLBC_CFG_LOG_DEFAULT_LOG_TO_CONSOLE                 1
// Default console log pattern: time [Logger Name][Log Level] - Message\n.
#define LLBC_CFG_LOG_DEFAULT_CONSOLE_LOG_PATTERN            "%T [%N][%L] - %m%n"
// Default console colourful output flag.
# define LLBC_CFG_LOG_DEFAULT_COLOURFUL_OUTPUT              1
// Default is not log to file.
#define LLBC_CFG_LOG_DEFAULT_LOG_TO_FILE                    0
// Default log file suffix.
#define LLBC_CFG_LOG_DEFAULT_LOG_FILE_SUFFIX                ".log"
// Default force application log path
#define LLBC_CFG_LOG_DEFAULT_FORCE_APP_LOG_PATH             false
// Default log code file path control config
#define LLBC_CFG_LOG_DEFAULT_LOG_CODE_FILE_PATH             false
// Default file log pattern: time file:line@[Logger Name][Log Level] - Message\n.
#define LLBC_CFG_LOG_DEFAULT_FILE_LOG_PATTERN               "%T %f:%l@[%N][%L] - %m%n"
// Default file rolling mode: no rolling.
#define LLBC_CFG_LOG_DEFAULT_FILE_ROLLING_MODE              0
// Default max log file size.
#define LLBC_CFG_LOG_MAX_FILE_SIZE                          LONG_MAX
// Default max backup file index.
#define LLBC_CFG_LOG_MAX_BACKUP_INDEX                       1000
// Default log file buffer size, in bytes.
#define LLBC_CFG_LOG_DEFAULT_LOG_FILE_BUFFER_SIZE           40960
// Default log appenders flush interval, in milli-seconds.
#define LLBC_CFG_LOG_DEFAULT_LOG_FLUSH_INTERVAL             200
// Default max log appenders flush interval, in milli-seconds.
#define LLBC_CFG_LOG_MAX_LOG_FLUSH_INTERVAL                 5000
// Default log using mode.
#define LLBC_CFG_LOG_USING_WITH_STREAM                      1
// Default take over config, only using in root logger, when a message log to 
// unconfiged logger, root logger will take over, if configed takeover to true.
#define LLBC_CFG_LOG_ROOT_LOGGER_TAKE_OVER_UNCONFIGED       1
// Default logfile create option
#define LLBC_CFG_LOG_LAZY_CREATE_LOG_FILE                   0
// Default log config item not config use default or root config(root/default).
#define LLBC_CFG_LOG_DEFAULT_NOT_CONFIG_OPTION_USE          "root"
// Log data object pool units size per block.
#define LLBC_CFG_LOG_LOG_DATA_OBJPOOL_UNIT_SIZE_PER_BLOCK   512

/**
 * \brief core/timer about configs.
 */
// Strict timer schedule mode.
#define LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE                 0
// Long timeout time, in milli-seconds, when a timer timeout time >= <this value>, when call Cancel(), will force remove from binary heap.
#define LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME               864000000 // 10 days

/**
* \brief core/objectpool about configs.
*/
// Object pool per-block units number define.
#define LLBC_CFG_CORE_OBJECT_POOL_BLOCK_UNITS_NUMBER        64
// Object pool statistic top N limit define.
#define LLBC_CFG_CORE_OBJECT_POOL_STAT_TOP_N                15
// Object pool memory allign config.
#if LLBC_64BIT_PROCESSOR
 #define LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN             8
#else
 #define LLBC_CFG_CORE_OBJECT_POOL_MEMORY_ALIGN             4
#endif
// Object pool debug option.
#define LLBC_CFG_CORE_OBJECT_POOL_DEBUG                     0
// Object reset match methods control.
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_clear      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Clear      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reset      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reset      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_reuse      1
#define LLBC_CFG_CORE_OBJECT_POOL_RESETOBJ_MATCH_Reuse      1
// Some llbc framework types object pool units number define.
#define LLBC_CFG_CORE_OBJECT_POOL_PACKET_UNITS_NUMBER        256 // LLBC_Packet
#define LLBC_CFG_CORE_OBJECT_POOL_MESSAGE_BLOCK_UNITS_NUMBER 256 // LLBC_MessageBlock

/**
 * \brief ObjBase about configs.
 */
// Dictionary default bucket size.
#define LLBC_CFG_OBJBASE_DICT_DFT_BUCKET_SIZE               100
// Dictionary string key hash algorithm(case insensitive).
// Supports: SDBM, RS, JS, PJW, ELF, BKDR, DJB, AP
// Default: BKDR
#define LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO                 BKDR

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
// Default socket send buffer size(0 means use system default and allow system dynamic adjust send buffer size, if supported).
#define LLBC_CFG_COMM_DFT_SOCK_SEND_BUF_SIZE                0
// Default socket recv buffer size(0 means use system default and allow system dynamic adjust recv buffer size, if supported).
#define LLBC_CFG_COMM_DFT_SOCK_RECV_BUF_SIZE                0
// Default session send buffer size(LLBC_INFINITE means no limit).
// Note:
// - this buffer size is send buffer size limit, if session will send data size greater than 
//   or equal to setting value, will trigger LLBC_ERROR_SESSION_SND_BUF_LIMIT error.
#define LLBC_CFG_COMM_DFT_SESSION_SEND_BUF_SIZE             LLBC_INFINITE
// Default session recv buffer size(not allow set to LLBC_INFINITE, is must be a actually size).
// Note:
// - this buffer size is initialize recv buffer size, if not enough to recv socket data, will auto expand.
#define LLBC_CFG_COMM_DFT_SESSION_RECV_BUF_SIZE             1024
// Default max packet size
// Note:
// - this packet size is PacketProcol limit, the size is only used for PacketProcol
#define LLBC_CFG_COMM_DFT_MAX_PACKET_SIZE                   LLBC_INFINITE
// Session recv buffer use object pool option, this option is performance option.
// Note: 
// - if enabled, can improvement read data from socket performance,
//   but once you turn on this option, your server memory will be streteched very large.
// - if enabled, LLBC_CFG_COMM_DFT_SESSION_RECV_BUF_SIZE will no effect.
#define LLBC_CFG_COMM_SESSION_RECV_BUF_USE_OBJ_POOL         0
// Message buffer element(block) allow resize limit.
#define LLBC_CFG_COMM_MSG_BUFFER_ELEM_RESIZE_LIMIT          (8 * 1024)
// Default service FPS value.
#define LLBC_CFG_COMM_DFT_SERVICE_FPS                       200
// Min service FPS value.
#define LLBC_CFG_COMM_MIN_SERVICE_FPS                       1
// Max service FPS value.
#define LLBC_CFG_COMM_MAX_SERVICE_FPS                       1000
// Sampler support option, default is true.
#define LLBC_CFG_COMM_ENABLE_SAMPLER_SUPPORT                1
// Per thread drive max services count.
#define LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT        16
// Determine enable the service has status handler support or not.
#define LLBC_CFG_COMM_ENABLE_STATUS_HANDLER                 1
// Determine enable the service has status desc support or not.
#define LLBC_CFG_COMM_ENABLE_STATUS_DESC                    1
// Determine enable the unify pre-subscribe handler support or not.
#define LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE             1
// Dynamic create comp create method prefix name.
#define LLBC_CFG_COMM_CREATE_COMP_FROM_LIB_FUNC_PREFIX      "llbc_create_comp_"
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
 #define LLBC_CFG_APP_DUMPFILE_DUMPTYPES            (MiniDumpWithFullMemory | \
                                                     MiniDumpWithFullMemoryInfo | \
                                                     MiniDumpIgnoreInaccessibleMemory | \
                                                     MiniDumpWithFullAuxiliaryState | \
                                                     MiniDumpWithHandleData | \
                                                     MiniDumpWithThreadInfo)
#endif
// Application try start interval(Call OnStart() interval), in milli-seconds.
#define LLBC_CFG_APP_TRY_START_INTERVAL             100
// Application try stop interval(Call OnStop() interval), in milli-seconds.
#define LLBC_CFG_APP_TRY_STOP_INTERVAL              100
// Application stop signals.
#if LLBC_TARGET_PLATFORM_WIN32
 #define LLBC_CFG_APP_STOP_SIGNALS                   {SIGINT, SIGTERM}
#else // Non-Win32
 #define LLBC_CFG_APP_STOP_SIGNALS                   {SIGINT, SIGTERM, SIGQUIT}
#endif // Win32
// Application config reload signals.
#if LLBC_TARGET_PLATFORM_WIN32
#define LLBC_CFG_APP_CFG_RELOAD_SIGNALS             {}
#else // Non-Win32
#define LLBC_CFG_APP_CFG_RELOAD_SIGNALS             {SIGUSR2}
#endif // Win32


