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

#include "llbc/core/thread/SpinLock.h"
#include "llbc/core/utils/Util_Delegate.h"
#include "llbc/core/objpool/ObjPool.h"

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogTrace.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_LoggerConfigInfo;
class LLBC_LogTimeAccessor;
class LLBC_BaseLogAppender;
class LLBC_LogRunnable;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The logger class encapsulation.
 */
class LLBC_EXPORT LLBC_Logger
{
public:
    LLBC_Logger();
    ~LLBC_Logger();

public:
    /**
     * Initialize the loggeer.
     * @param[in] config            - logger config info.
     * @param[in] sharedLogRunnable - the shared log runnable.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_LoggerConfigInfo *config, LLBC_LogRunnable *sharedLogRunnable);

    /**
     * Check logger initialized or not.
     * @return bool - return 0 if success, otherwise return -1.
     */
    bool IsInit() const;

    /**
     * Finalize logger.
     */
    void Finalize();

public:
    /**
     * Get the logger name.
     * @return const LLBC_String & - logger name.
     */
    const LLBC_String &GetLoggerName() const;

    /**
     * Get log level.
     * @return int - the log level.
     */
    int GetLogLevel() const;

    /**
     * @brief Set log level.
     * @param[in] appenderType - the log appender type.
     * @param[in] logLevel     - the log level(End will disabled logger).
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetAppenderLogLevel(int appenderType, int logLevel);

    /**
     * Get logger take over option, only available on root logger.
     * @return bool - take over option.
     */
    bool IsTakeOver() const;

    /**
     * Get asynchronous mode switch.
     * @return bool - asynchronous mode switch.
     */
    bool IsAsyncMode() const;

    /**
     * Get add timestamp in json log flag.
     * @return bool - add timestamp in json log flag.
     */
    bool IsAddTimestampInJsonLog() const;

    /**
     * Get log time accessor.
     * @return const LLBC_LogTimeAccessor & - the log time accessor.
     */
    const LLBC_LogTimeAccessor &GetLogTimeAccessor() const;

    /**
     * Get logger object pool.
     * @return const LLBC_ObjPool & - logger object pool.
     */
    const LLBC_ObjPool &GetLoggerObjPool() const;

public:
    /**
     * Get color log tag.
     * @return bool - the color log tag.
     */
    bool GetColorLogTag() const;

    /**
     * Add key and content to color log trace.
     * @param[in] logTrace - the log trace.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddColorLogTrace(const LLBC_LogTrace &logTrace);

    /**
     * Add log key and content to color log trace.
     * @param[in] traceKey     - the trace key.
     * @param[in] traceContent - the trace content.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _TraceKeyTy, typename _TraceContentTy>
    int AddColorLogTrace(const _TraceKeyTy &traceKey, const _TraceContentTy &traceContent);

    /**
     * Remove specific log color key contents.
     * @param[in] logTrace - the log trace.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveColorLogTrace(const LLBC_LogTrace &logTrace);

    /**
     * Remove specific log color key contents.
     * @param[in] traceKey     - the trace key.
     * @param[in] traceContent - the trace content.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _TraceKeyTy, typename _TraceContentTy>
    int RemoveColorLogTrace(const _TraceKeyTy &traceKey, const _TraceContentTy &traceContent);

    /**
     * Remove specific color key.
     * @param[in] traceKey - the trace key.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveColorLogKey(const LLBC_LogTrace::TraceKey &traceKey);

    /**
     * Clear specific key log traces.
     * @param[in] traceKey - the trace key.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _TraceKeyTy>
    int RemoveColorLogKey(const _TraceKeyTy &traceKey);

    /**
     * Clear all color log traces.
     */
    void ClearAllColorLogTraces();

public:
    /**
     * Set log hook.
     * @param[in] logLevel - the log level.
     * @param[in] logHook  - the log hook delegate, is is nullptr, will clear log hook.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetLogHook(int logLevel, const LLBC_Delegate<void(const LLBC_LogData *)> &logHook);

    /**
     * Set log hook.
     * @param[in] logLevels - the log levels.
     * @param[in] logHook   - the log hook delegate, is is nullptr, will clear log hook.
     * @return int - return 0 if success, otherwise return -1.
     */
    int SetLogHook(std::initializer_list<int> logLevels, const LLBC_Delegate<void(const LLBC_LogData *)> &logHook);

public:
    /**
     * Add log trace to logger.
     * @param[in] logTrace - the log trace.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddLogTrace(const LLBC_LogTrace &logTrace);

    /**
     * Add log trace to logger.
     * @param[in] traceKey     - the trace key.
     * @param[in] traceContent - the trace content.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _TraceKeyTy, typename _TraceContentTy>
    int AddLogTrace(const _TraceKeyTy &traceKey, const _TraceContentTy &traceContent);

    /**
     * Remove log trace from logger.
     * @param[in] logTrace            - the log trace.
     * @param[in] setTraceTimesToZero - set this log trace times to zero flag,
     *                                  if true, will set trace times to zero and remove this log trace.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveLogTrace(const LLBC_LogTrace &logTrace, bool setTraceTimesToZero);

    /**
     * Remove log trace from logger.
     * @param[in] traceKey            - the trace key.
     * @param[in] traceContent        - the trace content.
     * @param[in] setTraceTimesToZero - set this log trace times to zero flag,
     *                                  if true, will set trace times to zero and remove this log trace.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename _TraceKeyTy, typename _TraceContentTy>
    int RemoveLogTrace(const _TraceKeyTy &traceKey,
                       const _TraceContentTy &traceContent,
                       bool setTraceTimesToZero);

    /**
     * Get log trace times.
     * @param logTrace - the log trace obj.
     * @return size_t - this log trace obj trace times.
     */
    size_t GetLogTraceTimes(const LLBC_LogTrace &logTrace) const;

    /**
     * Get log trace times.
     * @param[in] traceKey     - the trace key.
     * @param[in] traceContent - the trace content.
     * @return size_t - this log trace obj trace times.
     */
     template <typename _TraceKeyTy, typename _TraceContentTy>
    size_t GetLogTraceTimes(const _TraceKeyTy &traceKey, const _TraceContentTy &traceContent) const;

    /**
     * Clear specific key log traces.
     * @param[in] traceKey - the trace key.
     */
    void ClearLogTrace(const LLBC_LogTrace::TraceKey &traceKey);

    /**
     * Clear specific key log traces.
     * @param[in] traceKey - the trace key.
     */
    template <typename _TraceKeyTy>
    void ClearLogTrace(const _TraceKeyTy &traceKey);

    /**
     * Clear all log traces from logger.
     */
    void ClearAllLogTraces();

public:
    /**
     * Output specific level message.
     * @param[in] tag  - log tag, can set to nullptr.
     * @param[in] file - log file name.
     * @param[in] line - log file line.
     * @param[in] func - log function.
     * @param[in] fmt  - format control string.
     * @param[in] ...  - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Debug(const char *tag,
              const char *file,
              int line,
              const char *func,
              const char *fmt,
              ...) LLBC_STRING_FORMAT_CHECK(6, 7);
    int Trace(const char *tag,
              const char *file,
              int line,
              const char *func,
              const char *fmt,
              ...) LLBC_STRING_FORMAT_CHECK(6, 7);
    int Info(const char *tag,
             const char *file,
             int line,
             const char *func,
             const char *fmt,
             ...) LLBC_STRING_FORMAT_CHECK(6, 7);
    int Warn(const char *tag,
             const char *file,
             int line,
             const char *func,
             const char *fmt,
             ...) LLBC_STRING_FORMAT_CHECK(6, 7);
    int Error(const char *tag,
              const char *file,
              int line,
              const char *func,
              const char *fmt,
              ...) LLBC_STRING_FORMAT_CHECK(6, 7);
    int Fatal(const char *tag,
              const char *file,
              int line,
              const char *func,
              const char *fmt,
              ...) LLBC_STRING_FORMAT_CHECK(6, 7);

    /**
     * Output fmt using given level.
     * @param[in] level  - log level.
     * @param[in] tag    - log tag, can set to nullptr.
     * @param[in] file   - log file name.
     * @param[in] line   - log file line.
     * @param[in] func   - log function.
     * @param[in] fmt    - format control string.
     * @param[in] ...    - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Output(int level,
               const char *tag,
               const char *file,
               int line,
               const char *func,
               const char *fmt,
               ...) LLBC_STRING_FORMAT_CHECK(7, 8);

    /**
     * Output message by va_list.
     * @param[in] level - log level.
     * @param[in] tag   - log tag, can set to nullptr.
     * @param[in] file  - log file name.
     * @param[in] line  - log file line.
     * @param[in] func  - log function.
     * @param[in] fmt   - format control string.
     * @param[in] va    - variadic parameter list.
     * @return int - return 0 if success, otherwise return -1.
     */
    int VOutput(int level,
                const char *tag,
                const char *file,
                int line,
                const char *func,
                const char *fmt,
                va_list va);

    /**
     * Like Output() method, but message is non-format message, use to improve performance.
     * @param[in] level  - log level.
     * @param[in] tag    - log tag, can set to nullptr.
     * @param[in] file   - log file name.
     * @param[in] line   - log file line.
     * @param[in] func   - log function.
     * @param[in] time   - log time, in micro-seconds.
     * @param[in] msg    - message string.
     * @param[in] msgLen - message string length, if -1, will auto calculate.
     * @return int - return 0 if success, otherwise return -1.
     */
    int NonFormatOutput(int level,
                        const char *tag,
                        const char *file,
                        int line,
                        const char *func,
                        sint64 time,
                        const char *msg,
                        size_t msgLen);

private:
    /**
     * Build log data by format control string and variable parameter list.
     * @param[in] level - log level.
     * @param[in] tag   - log tag.
     * @param[in] file  - log file name.
     * @param[in] line  - log file line.
     * @param[in] func  - log function.
     * @param[in] fmt   - log format control string.
     * @param[in] va    - the message variable parameter list.
     * @return LLBC_LogData * - the log data.
     */
    LLBC_LogData *BuildLogData(int level,
                               const char *tag,
                               const char *file,
                               int line,
                               const char *func,
                               const char *fmt,
                               va_list va);

    /**
     * Build log data by msg and length.
     * @param[in] level  - log level.
     * @param[in] tag    - log tag.
     * @param[in] file   - log file name.
     * @param[in] line   - log file line.
     * @param[in] func   - log function.
     * @param[in] time   - log time, in micro-seconds.
     * @param[in] msg    - log message.
     * @param[in] msgLen - log message length.
     */
    LLBC_LogData *BuildLogData(int level,
                               const char *tag,
                               const char *file,
                               int line,
                               const char *func,
                               sint64 time,
                               const char *msg,
                               size_t msgLen);

    /**
     * Fill log data non-msg members.
     * @param[in] level   - log level.
     * @param[in] tag     - log tag.
     * @param[in] file    - log file name.
     * @param[in] line    - log file line.
     * @param[in] func    - log function.
     * @param[in] time    - log time.
     * @param[in] logData - log data.
     * @param[in] libTls  - log tls.
     */
    void FillLogDataNonMsgMembers(int level,
                                  const char *tag,
                                  const char *file,
                                  int line,
                                  const char *func,
                                  sint64 time,
                                  LLBC_LogData *logData,
                                  __LLBC_LibTls *libTls);

private:
    /**
     * Friend classs: LLBC_LogRunnable.
     * Asset method/data-members:
     * - OutputLogData(const LLBC_LogData &data):int
     * - Flush(bool force, sint64 now):void
     */
    friend class LLBC_LogRunnable;

    /**
     * Add log appender.
     * @param[in] appender - log appender.
     */
    void AddAppender(LLBC_BaseLogAppender *appender);

    /**
     * Output log data.
     * @param[in] data - log data.
     */
    int OutputLogData(const LLBC_LogData &data);

    /**
    * Flush logger(for now, just only need flush all appenders).
    * @param[in] force - force or not.
    * @param[in] now   - now time in milli-seconds.
    */
    void Flush(bool force, sint64 now);

    /**
     * Flush appenders.
     */
    void FlushAppenders();

    /**
     * Clear logger non-runnable data members.
     * @param[in] keepErrNo - keep errno(included sub errno) or not, default is true.
     */
    void ClearNonRunnableMembers(bool keepErrNo = true);

private:
    // Logger name.
    LLBC_String _name;
    // Logger lock.
    mutable LLBC_SpinLock _lock;

    // Log level.
    volatile int _logLevel;
    // Logger config.
    LLBC_LoggerConfigInfo *_config;

    // Log time accessor.
    const LLBC_LogTimeAccessor *_logTimeAccessor;

    // Log trace manager.
    LLBC_LogTraceMgr *_logTraceMgr;

    // Log runnable object.
    LLBC_LogRunnable *_logRunnable;

    // Last flush time, in milliseconds.
    sint64 _lastFlushTime;
    // Flush interval, in milliseconds.
    sint64 _flushInterval;
    // Log appenders.
    LLBC_BaseLogAppender *_appenders;

    // Logger object pool.
    LLBC_ObjPool _objPool;
    // LLBC_LogData typed object pool.
    LLBC_TypedObjPool<LLBC_LogData> &_logDataTypedObjPool;

    // Log hooks.
    LLBC_Delegate<void(const LLBC_LogData *)> _logHooks[LLBC_LogLevel::End];
};

__LLBC_NS_END

#include "llbc/core/log/LoggerInl.h"



