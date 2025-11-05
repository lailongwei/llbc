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

#include "llbc/core/singleton/Singleton.h"

#include "llbc/core/thread/Guard.h"
#include "llbc/core/thread/FastLock.h"
#include "llbc/core/thread/DummyLock.h"

#include "llbc/core/log/LogTimeAccessor.h"
#include "llbc/core/log/Logger.h"

#if LLBC_CFG_LOG_USING_WITH_STREAM
#include "llbc/core/log/LogMessageBuffer.h"
#endif // LLBC_CFG_LOG_USING_WITH_STREAM

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Logger;
class LLBC_LogJsonMsg;
class LLBC_LogRunnable;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The logger manager class encapsulation.
 */
class LLBC_EXPORT LLBC_LoggerMgr
{
public:
    LLBC_LoggerMgr();
    ~LLBC_LoggerMgr();

public:
    /**
     * Initialize logger manager using config file.
     * @param[in] cfgFilePath   - config file, for now, supported properties/xml format.
     * @param[in] logTimeOffset - the log time offset, default is zero.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &cfgFilePath,
                   const LLBC_TimeSpan &logTimeOffset = LLBC_TimeSpan::zero);

    /**
     * Reload logger manager using config file.
     * Note: logger reload only support existing logger(s) reload.
     * @param[in] newCfgFilePath = the new config, if is empty, use original config file.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Reload(const LLBC_String &newCfgFilePath = "");

    /**
     * Finalize logger manager.
     */
    void Finalize();

    /**
     * Check logger manager inited or not.
     * @return bool - the inited flag.
     */
    bool IsInited() const;

    /**
     * Get root logger.
     * @return LLBC_Logger * - root logger.
     */
    LLBC_Logger *GetRootLogger() const;

    /**
     * Get logger by name.
     * @param[in] name - logger name.
     * @return LLBC_Logger * - logger.
     */
    LLBC_Logger *GetLogger(const LLBC_CString &name) const;

    /**
     * Uninit output.
     * @param[in] logLv - the log level.
     * @param[in] tag   - the log tag.
     * @param[in] file  - the log file.
     * @param[in] line  - the log file line number.
     * @param[in] func  - the log function.
     * @param[in] fmt   -  format control string.
     * @param[in] ...   - optional arguments.
     */
    void UnInitOutput(int logLv,
                      const char *tag,
                      const char *file,
                      int line,
                      const char *func,
                      const char *fmt,
                      ...) LLBC_STRING_FORMAT_CHECK(7, 8);

    /**
     * Uninit non format output.
     * @param[in] logLv  - the log level.
     * @param[in] tag    - the log tag.
     * @param[in] file  - the log file.
     * @param[in] line  - the log file line number.
     * @param[in] func  - the log function.
     * @param[in] msg    - the log message.
     * @param[in] msgLen - the log message length.
     */
    void UnInitNonFormatOutput(int logLv,
                               const char *tag,
                               const char *file,
                               int line,
                               const char *func,
                               const char *msg,
                               size_t msgLen);

public:
    /**
     * Get log time accessor.
     * @return const LLBC_LogTimeAccessor & - the log time accessor.
     */
    const LLBC_LogTimeAccessor &GetLogTimeAccessor() const;

private:
    mutable LLBC_DummyLock _lock;

    LLBC_String _cfgFilePath;
    LLBC_LogTimeAccessor _logTimeAccessor;
    LLBC_LogRunnable *_sharedLogRunnable;

    LLBC_Logger * volatile _rootLogger;
    std::map<LLBC_CString, LLBC_Logger *> _cstr2Loggers;
    std::vector<std::pair<LLBC_CString, LLBC_Logger *> > _loggerList;
    std::map<LLBC_CString, LLBC_Logger *>::const_iterator _cstr2LoggersEnd;

    static LLBC_FastLock _uninitColorfulOutputLock;
};

__LLBC_NS_END

/**
 * Singleton class macro define.
 */
template class LLBC_EXPORT LLBC_NS LLBC_Singleton<LLBC_NS LLBC_LoggerMgr>;
#define LLBC_LoggerMgrSingleton LLBC_NS LLBC_Singleton<LLBC_NS LLBC_LoggerMgr>::Instance()

/**
 * @brief The llbc library log macro define.
 * @param[in] loggerName - the logger name, nullptr if log to root.
 * @param[in] tag        - the log tag, nullptr if no tag.
 * @param[in] level      - log level.
 * @param[in] fmt        - the log message format string.
 * @param[in] ...        - the variadic log message parameters.
 */
#define LLOG(loggerName, tag, level, fmt, ...)                              \
    do {                                                                    \
        auto __loggerMgr__ = LLBC_LoggerMgrSingleton;                       \
        if (LIKELY(__loggerMgr__->IsInited())) {                            \
            LLBC_NS LLBC_Logger *__l__;                                     \
            if (loggerName != nullptr) {                                    \
                __l__ = __loggerMgr__->GetLogger(loggerName);               \
                if (UNLIKELY(__l__ == nullptr))                             \
                    break;                                                  \
            }                                                               \
            else {                                                          \
                __l__ = __loggerMgr__->GetRootLogger();                     \
            }                                                               \
            if ((level) < __l__->GetLogLevel() && !__l__->GetLogColorTag()) \
                break;                                                      \
                                                                            \
            __l__->Output(level,                                            \
                          tag,                                              \
                          __FILE__,                                         \
                          __LINE__,                                         \
                          __FUNCTION__,                                     \
                          fmt,                                              \
                          ##__VA_ARGS__);                                   \
        }                                                                   \
        else {                                                              \
            __loggerMgr__->UnInitOutput(level,                              \
                                        tag,                                \
                                        __FILE__,                           \
                                        __LINE__,                           \
                                        __FUNCTION__,                       \
                                        fmt,                                \
                                        ##__VA_ARGS__);                     \
        }                                                                   \
    } while (false)                                                         \

#define LLOG_DEBUG(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)

#define LLOG_TRACE(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)
#define LLOG_TRACE2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)
#define LLOG_TRACE3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)
#define LLOG_TRACE4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)

#define LLOG_INFO(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Info, fmt, ##__VA_ARGS__)
#define LLOG_INFO2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Info, fmt, ##__VA_ARGS__)
#define LLOG_INFO3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Info, fmt, ##__VA_ARGS__)
#define LLOG_INFO4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Info, fmt, ##__VA_ARGS__)

#define LLOG_WARN(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Warn, fmt, ##__VA_ARGS__)
#define LLOG_WARN2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Warn, fmt, ##__VA_ARGS__)
#define LLOG_WARN3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Warn, fmt, ##__VA_ARGS__)
#define LLOG_WARN4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Warn, fmt, ##__VA_ARGS__)

#define LLOG_ERROR(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Error, fmt, ##__VA_ARGS__)
#define LLOG_ERROR2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Error, fmt, ##__VA_ARGS__)
#define LLOG_ERROR3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Error, fmt, ##__VA_ARGS__)
#define LLOG_ERROR4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Error, fmt, ##__VA_ARGS__)

#define LLOG_FATAL(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Fatal, fmt, ##__VA_ARGS__)
#define LLOG_FATAL2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Fatal, fmt, ##__VA_ARGS__)
#define LLOG_FATAL3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Fatal, fmt, ##__VA_ARGS__)
#define LLOG_FATAL4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Fatal, fmt, ##__VA_ARGS__)

/**
 * The llbc library json log macro define.
 * @param[in] loggerName - the logger name, nullptr if log to root.
 * @param[in] tag        - the log tag, nullptr if no tag.
 * @param[in] level      - log level.
 */
#define LJLOG(loggerName, tag, level)                    \
    (LLBC_NS LLBC_LogJsonMsg(                            \
                   LIKELY(LLBC_LoggerMgrSingleton->IsInited()) ? \
                        (loggerName != nullptr ?         \
                            LLBC_LoggerMgrSingleton->GetLogger(loggerName) : \
                                LLBC_LoggerMgrSingleton->GetRootLogger()) : nullptr, \
                   tag,                                  \
                   level,                                \
                   __FILE__,                             \
                   __LINE__,                             \
                   __FUNCTION__))                        \

#define LJLOG_DEBUG() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Debug)
#define LJLOG_DEBUG2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Debug)
#define LJLOG_DEBUG3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Debug)
#define LJLOG_DEBUG4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Debug)

#define LJLOG_TRACE() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Trace)
#define LJLOG_TRACE2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Trace)
#define LJLOG_TRACE3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Trace)
#define LJLOG_TRACE4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Trace)

#define LJLOG_INFO() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Info)
#define LJLOG_INFO2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Info)
#define LJLOG_INFO3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Info)
#define LJLOG_INFO4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Info)

#define LJLOG_WARN() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Warn)
#define LJLOG_WARN2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Warn)
#define LJLOG_WARN3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Warn)
#define LJLOG_WARN4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Warn)

#define LJLOG_ERROR() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Error)
#define LJLOG_ERROR2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Error)
#define LJLOG_ERROR3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Error)
#define LJLOG_ERROR4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Error)

#define LJLOG_FATAL() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Fatal)
#define LJLOG_FATAL2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Fatal)
#define LJLOG_FATAL3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Fatal)
#define LJLOG_FATAL4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Fatal)

/**
 * Log trace add macro.
 */
#define LLOG_ADD_TRACE(traceKey, traceContent) LLOG_ADD_TRACE2(nullptr, traceKey, traceContent)
#define LLOG_ADD_TRACE2(loggerName, traceKey, traceContent)                                     \
    const LLBC_NS LLBC_LogTrace LLBC_Concat(__logTrace__, __LINE__)(traceKey, traceContent);    \
    LLBC_NS LLBC_Logger *LLBC_Concat(__logTraceLogger__, __LINE__) = nullptr;                   \
    auto LLBC_Concat(__logTraceLoggerMgr__, __LINE__) = LLBC_LoggerMgrSingleton;                \
                                                                                                \
    LLBC_NS LLBC_InvokeGuard LLBC_Concat(__logTraceRemovGuard__, __LINE__)([                    \
        &LLBC_Concat(__logTrace__, __LINE__),                                                   \
        &LLBC_Concat(__logTraceLogger__, __LINE__)] () {                                        \
            if (LLBC_Concat(__logTraceLogger__, __LINE__))                                      \
                LLBC_Concat(__logTraceLogger__, __LINE__)->RemoveLogTrace(LLBC_Concat(__logTrace__, __LINE__), false); \
    });                                                                                         \
                                                                                                \
    if (LIKELY(LLBC_Concat(__logTraceLoggerMgr__, __LINE__)->IsInited())) {                     \
        if (loggerName != nullptr)                                                              \
            LLBC_Concat(__logTraceLogger__, __LINE__) = LLBC_Concat(__logTraceLoggerMgr__, __LINE__)->GetLogger(loggerName); \
        else                                                                                    \
            LLBC_Concat(__logTraceLogger__, __LINE__) = LLBC_Concat(__logTraceLoggerMgr__, __LINE__)->GetRootLogger(); \
                                                                                                \
        if (LIKELY(LLBC_Concat(__logTraceLogger__, __LINE__)))                                  \
            LLBC_Concat(__logTraceLogger__, __LINE__)->AddLogTrace(LLBC_Concat(__logTrace__, __LINE__)); \
    }                                                                                           \

#if LLBC_CFG_LOG_USING_WITH_STREAM
/**
 * The llbc library stream log macro define.
 * @param[in] loggerName - the logger name, nullptr if log to root.
 * @param[in] tag        - the log tag, nullptr if no tag.
 * @param[in] level      - log level.
 * @param[in] streamMsg  - stream message, like: '"the log message, intVal:" << 3 <<", strVal:" << "strMsg"'
 */
#define LSLOG(loggerName, tag, level, streamMsg)               \
    do {                                                       \
        auto __loggerMgr__ = LLBC_LoggerMgrSingleton;          \
        if (LIKELY(__loggerMgr__->IsInited())) {               \
            LLBC_NS LLBC_Logger *__l__;                        \
            if (loggerName != nullptr) {                       \
                __l__ = __loggerMgr__->GetLogger(loggerName);  \
                if (UNLIKELY(__l__ == nullptr))                \
                    break;                                     \
            }                                                  \
            else {                                             \
                __l__ = __loggerMgr__->GetRootLogger();        \
            }                                                  \
                                                               \
            if (level < __l__->GetLogLevel())                  \
                break;                                         \
                                                               \
            LLBC_NS LLBC_LogMessageBuffer __lmb__;             \
            const LLBC_String &formattedLogMsg = __lmb__.str(__lmb__ << streamMsg); \
            __l__->NonFormatOutput(level,                      \
                                   tag,                        \
                                   __FILE__,                   \
                                   __LINE__,                   \
                                   __FUNCTION__,               \
                                   0ll,                        \
                                   formattedLogMsg.c_str(),    \
                                   formattedLogMsg.length());  \
        }                                                      \
        else {                                                 \
            LLBC_NS LLBC_LogMessageBuffer __lmb__;             \
            LLBC_LoggerMgrSingleton->UnInitOutput(             \
                level,                                         \
                tag,                                           \
                __FILE__,                                      \
                __LINE__,                                      \
                __FUNCTION__,                                  \
                "%s",                                          \
                __lmb__.str(__lmb__ << streamMsg).c_str());    \
        }                                                      \
    } while (false)                                            \

#define LSLOG_DEBUG(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Debug, streamMsg)
#define LSLOG_DEBUG2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Debug, streamMsg)
#define LSLOG_DEBUG3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Debug, streamMsg)
#define LSLOG_DEBUG4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Debug, streamMsg)

#define LSLOG_TRACE(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Trace, streamMsg)
#define LSLOG_TRACE2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Trace, streamMsg)
#define LSLOG_TRACE3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Trace, streamMsg)
#define LSLOG_TRACE4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Trace, streamMsg)

#define LSLOG_INFO(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Info, streamMsg)
#define LSLOG_INFO2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Info, streamMsg)
#define LSLOG_INFO3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Info, streamMsg)
#define LSLOG_INFO4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Info, streamMsg)

#define LSLOG_WARN(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Warn, streamMsg)
#define LSLOG_WARN2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Warn, streamMsg)
#define LSLOG_WARN3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Warn, streamMsg)
#define LSLOG_WARN4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Warn, streamMsg)

#define LSLOG_ERROR(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Error, streamMsg)
#define LSLOG_ERROR2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Error, streamMsg)
#define LSLOG_ERROR3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Error, streamMsg)
#define LSLOG_ERROR4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Error, streamMsg)

#define LSLOG_FATAL(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Fatal, streamMsg)
#define LSLOG_FATAL2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Fatal, streamMsg)
#define LSLOG_FATAL3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Fatal, streamMsg)
#define LSLOG_FATAL4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Fatal, streamMsg)

#endif // LLBC_CFG_LOG_USING_WITH_STREAM

__LLBC_INTERNAL_NS_BEGIN

/**
 * Log operator for condition judge helper macros.
 */
template<int ArgCount>
class __LLBC_ConditionLogOperator
{
public:
    static void Output(const char *logger,
                       const char *fileName,
                       int lineNo,
                       const char *funcName,
                       const char *logTag,
                       int logLv,
                       const char *fmt1,
                       const char *cond,
                       const char *behav,
                       const char *fmt2,
                       ...) LLBC_STRING_FORMAT_CHECK(10, 11)
    {
         // Formatting format 1 and format 2.
        LLBC_NS __LLBC_LibTls *libTls = LLBC_NS __LLBC_GetLibTls();
        int fmt1Len = snprintf(libTls->coreTls.loggerFmtBuf,
                               sizeof(libTls->coreTls.loggerFmtBuf),
                               fmt1, cond, behav);

        // Check format is valid.
        LLBC_ReturnIf(UNLIKELY(fmt1Len < 0), void());

        // Ensure fmt1Len not exceed buffer size.
        fmt1Len = std::min(fmt1Len, static_cast<int>(sizeof(libTls->coreTls.loggerFmtBuf) - 1));

        va_list ap;
        va_start(ap, fmt2);
        int fmt2Len = vsnprintf(libTls->coreTls.loggerFmtBuf + fmt1Len,
                                sizeof(libTls->coreTls.loggerFmtBuf) - fmt1Len,
                                fmt2, ap);
        va_end(ap);

        // Check format is valid.
        LLBC_ReturnIf(UNLIKELY(fmt2Len < 0), void());

        // Ensure fmt2Len not exceed buffer size.
        fmt2Len = std::min(fmt2Len, static_cast<int>(sizeof(libTls->coreTls.loggerFmtBuf) - fmt1Len - 1));

        // Calculate total length.
        int totalLen = fmt1Len + fmt2Len;

        // Output to spec logger(or exec uninit output).
        auto loggerMgr = LLBC_LoggerMgrSingleton;
        if (LIKELY(loggerMgr->IsInited()))
        {
            auto l = loggerMgr->GetLogger(logger);
            if (l && logLv >= l->GetLogLevel())
            {
                l->NonFormatOutput(logLv,
                                   logTag,
                                   fileName,
                                   lineNo,
                                   funcName,
                                   l->GetLogTimeAccessor().NowInMicroseconds(),
                                   libTls->coreTls.loggerFmtBuf,
                                   totalLen);
            }
        }
        else
        {
            loggerMgr->UnInitNonFormatOutput(logLv,
                                             logTag,
                                             fileName,
                                             lineNo,
                                             funcName,
                                             libTls->coreTls.loggerFmtBuf,
                                             totalLen);
        }
    }
};

template<>
class __LLBC_ConditionLogOperator<0>
{
public:
    static void Output(const char *logger,
                       const char *fileName,
                       int lineNo,
                       const char *funcName,
                       const char *logTag,
                       int logLv,
                       const char *fmt,
                       const char *cond,
                       const char *behav)
    {
        auto loggerMgr = LLBC_LoggerMgrSingleton;
        if (LIKELY(loggerMgr->IsInited()))
        {
            auto l = loggerMgr->GetLogger(logger);
            if (l && logLv >= l->GetLogLevel())
                l->Output(logLv, logTag, fileName, lineNo, funcName, fmt, cond, behav);
        }
        else
        {
            loggerMgr->UnInitOutput(logLv, logTag, fileName, lineNo, funcName, fmt, cond, behav);
        }
    }
};

__LLBC_INTERNAL_NS_END

/**
 * Condition LOG AND DO SOMETHING support macros.
 */
#define LLBC_LogAndDoIf(cond, logLv, behav, ...) LLBC_LogAndDoIf4(cond, nullptr, nullptr, logLv, behav, ##__VA_ARGS__)
#define LLBC_LogAndDoIf2(cond, logger, logLv, behav, ...) LLBC_LogAndDoIf4(cond, logger, nullptr, logLv, behav, ##__VA_ARGS__)
#define LLBC_LogAndDoIf3(cond, logTag, logLv, behav, ...) LLBC_LogAndDoIf4(cond, nullptr, logTag, logLv, behav, ##__VA_ARGS__)
#define LLBC_LogAndDoIf4(cond, logger, logTag, logLv, behav, ...)                            \
    {                                                                                        \
        if ((cond)) {                                                                        \
            LLBC_INL_NS __LLBC_ConditionLogOperator<                                         \
                    std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output( \
                logger,                                                                      \
                __FILE__, __LINE__, __FUNCTION__,                                            \
                logTag,                                                                      \
                LLBC_NS LLBC_LogLevel::logLv,                                                \
                "LLBC_DoIf:<\"%s\"> is true, do:%s. ",                                       \
                #cond, #behav, ##__VA_ARGS__);                                               \
            behav;                                                                           \
        }                                                                                    \
    }                                                                                        \

/**
 * Condition LOG AND CONTINUE support macros.
 */
#define LLBC_LogAndContinueIf(cond, logLv, ...) LLBC_LogAndContinueIf4(cond, nullptr, nullptr, logLv, ##__VA_ARGS__)
#define LLBC_LogAndContinueIf2(cond, logger, logLv, ...) LLBC_LogAndContinueIf4(cond, logger, nullptr, logLv, ##__VA_ARGS__)
#define LLBC_LogAndContinueIf3(cond, logTag, logLv, ...) LLBC_LogAndContinueIf4(cond, nullptr, logTag, logLv, ##__VA_ARGS__)
#define LLBC_LogAndContinueIf4(cond, logger, logTag, logLv, ...)                             \
    {                                                                                        \
        if ((cond))                                                                          \
        {                                                                                    \
            LLBC_INL_NS __LLBC_ConditionLogOperator<                                         \
                    std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output( \
                logger,                                                                      \
                __FILE__, __LINE__, __FUNCTION__,                                            \
                logTag,                                                                      \
                LLBC_NS LLBC_LogLevel::logLv,                                                \
                "LLBC_ContinueIf:<\"%s\"> is true. %s",                                      \
                #cond, "", ##__VA_ARGS__);                                                   \
            continue;                                                                        \
        }                                                                                    \
    }                                                                                        \

/**
 * Condition LOG AND BREAK support macros.
 */
#define LLBC_LogAndBreakIf(cond, logLv, ...) LLBC_LogAndBreakIf4(cond, nullptr, nullptr, logLv, ##__VA_ARGS__)
#define LLBC_LogAndBreakIf2(cond, logger, logLv, ...) LLBC_LogAndBreakIf4(cond, logger, nullptr, logLv, ##__VA_ARGS__)
#define LLBC_LogAndBreakIf3(cond, logTag, logLv, ...) LLBC_LogAndBreakIf4(cond, nullptr, logTag, logLv, ##__VA_ARGS__)
#define LLBC_LogAndBreakIf4(cond, logger, logTag, logLv, ...)                                \
    {                                                                                        \
        if ((cond))                                                                          \
        {                                                                                    \
            LLBC_INL_NS __LLBC_ConditionLogOperator<                                         \
                    std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output( \
                logger,                                                                      \
                __FILE__, __LINE__, __FUNCTION__,                                            \
                logTag,                                                                      \
                LLBC_NS LLBC_LogLevel::logLv,                                                \
                "LLBC_LogAndBreakIf:<\"%s\"> is true. %s",                                   \
                #cond, "", ##__VA_ARGS__);                                                   \
            break;                                                                           \
        }                                                                                    \
    }                                                                                        \

/**
 * Condition LOG AND RETURN support macros.
 */
#define LLBC_LogAndReturnIf(cond, logLv, ret, ...) LLBC_LogAndReturnIf4(cond, nullptr, nullptr, logLv, ret, ##__VA_ARGS__)
#define LLBC_LogAndReturnIf2(cond, logger, logLv, ret, ...) LLBC_LogAndReturnIf4(cond, logger, nullptr, logLv, ret, ##__VA_ARGS__)
#define LLBC_LogAndReturnIf3(cond, logTag, logLv, ret, ...) LLBC_LogAndReturnIf4(cond, nullptr, logTag, logLv, ret, ##__VA_ARGS__)
#define LLBC_LogAndReturnIf4(cond, logger, logTag, logLv, ret, ...)                          \
    {                                                                                        \
        if ((cond)) {                                                                        \
            LLBC_INL_NS __LLBC_ConditionLogOperator<                                         \
                    std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output( \
                logger,                                                                      \
                __FILE__, __LINE__, __FUNCTION__,                                            \
                logTag,                                                                      \
                LLBC_NS LLBC_LogLevel::logLv,                                                \
                "LLBC_ReturnIf:<\"%s\"> is true, return:%s. ",                               \
                #cond, #ret, ##__VA_ARGS__);                                                 \
            return ret;                                                                      \
        }                                                                                    \
    }                                                                                        \

/**
 * Condition LOG AND EXIT support macros.
 */
#define LLBC_LogAndExitIf(cond, logLv, exitCode, ...) LLBC_LogAndExitIf4(cond, nullptr, nullptr, logLv, exitCode, ...)
#define LLBC_LogAndExitIf2(cond, logger, logLv, exitCode, ...) LLBC_LogAndExitIf4(cond, logger, nullptr, logLv, exitCode, ...)
#define LLBC_LogAndExitIf3(cond, logTag, logLv, exitCode, ...) LLBC_LogAndExitIf4(cond, nullptr, logTag, logLv, exitCode, ...)
#define LLBC_LogAndExitIf4(cond, logger, logTag, logLv, exitCode, ...)                       \
    {                                                                                        \
        if ((cond)) {                                                                        \
            LLBC_INL_NS __LLBC_ConditionLogOperator<                                         \
                    std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output( \
                logger,                                                                      \
                __FILE__, __LINE__, __FUNCTION__,                                            \
                logTag,                                                                      \
                LLBC_NS LLBC_LogLevel::logLv,                                                \
                "LLBC_ExitIf:<\"%s\"> is true, exitCode:%d. ",                               \
                #cond, static_cast<int>(exitCode), ##__VA_ARGS__);                           \
                                                                                             \
            if (LLBC_LoggerMgrSingleton->IsInited())                                         \
                LLBC_LoggerMgrSingleton->Finalize();                                         \
                                                                                             \
            ::exit(static_cast<int>(exitCode));                                              \
        }                                                                                    \
    }                                                                                        \

/**
 * <LOG LV> AND DO IF macros.
 */
#define LLBC_DebugAndDoIf(cond, behav, ...) LLBC_LogAndDoIf(cond, Debug, behav, ##__VA_ARGS__)
#define LLBC_DebugAndDoIf2(cond, logger, behav, ...) LLBC_LogAndDoIf2(cond, logger, Debug, behav, ##__VA_ARGS__)
#define LLBC_DebugAndDoIf3(cond, logTag, behav, ...) LLBC_LogAndDoIf3(cond, logTag, Debug, behav, ##__VA_ARGS__)
#define LLBC_DebugAndDoIf4(cond, logger, logTag, behav, ...) LLBC_LogAndDoIf4(cond, logger, logTag, Debug, behav, ##__VA_ARGS__)
#define LLBC_TraceAndDoIf(cond, behav, ...) LLBC_LogAndDoIf(cond, Trace, behav, ##__VA_ARGS__)
#define LLBC_TraceAndDoIf2(cond, logger, behav, ...) LLBC_LogAndDoIf2(cond, logger, Trace, behav, ##__VA_ARGS__)
#define LLBC_TraceAndDoIf3(cond, logTag, behav, ...) LLBC_LogAndDoIf3(cond, logTag, Trace, behav, ##__VA_ARGS__)
#define LLBC_TraceAndDoIf4(cond, logger, logTag, behav, ...) LLBC_LogAndDoIf4(cond, logger, logTag, Trace, behav, ##__VA_ARGS__)
#define LLBC_InfoAndDoIf(cond, behav, ...) LLBC_LogAndDoIf(cond, Info, behav, ##__VA_ARGS__)
#define LLBC_InfoAndDoIf2(cond, logger, behav, ...) LLBC_LogAndDoIf2(cond, logger, Info, behav, ##__VA_ARGS__)
#define LLBC_InfoAndDoIf3(cond, logTag, behav, ...) LLBC_LogAndDoIf3(cond, logTag, Info, behav, ##__VA_ARGS__)
#define LLBC_InfoAndDoIf4(cond, logger, logTag, behav, ...) LLBC_LogAndDoIf4(cond, logger, logTag, Info, behav, ##__VA_ARGS__)
#define LLBC_WarnAndDoIf(cond, behav, ...) LLBC_LogAndDoIf(cond, Warn, behav, ##__VA_ARGS__)
#define LLBC_WarnAndDoIf2(cond, logger, behav, ...) LLBC_LogAndDoIf2(cond, logger, Warn, behav, ##__VA_ARGS__)
#define LLBC_WarnAndDoIf3(cond, logTag, behav, ...) LLBC_LogAndDoIf3(cond, logTag, Warn, behav, ##__VA_ARGS__)
#define LLBC_WarnAndDoIf4(cond, logger, logTag, behav, ...) LLBC_LogAndDoIf4(cond, logger, logTag, Warn, behav, ##__VA_ARGS__)
#define LLBC_ErrorAndDoIf(cond, behav, ...) LLBC_LogAndDoIf(cond, Error, behav, ##__VA_ARGS__)
#define LLBC_ErrorAndDoIf2(cond, logger, behav, ...) LLBC_LogAndDoIf2(cond, logger, Error, behav, ##__VA_ARGS__)
#define LLBC_ErrorAndDoIf3(cond, logTag, behav, ...) LLBC_LogAndDoIf3(cond, logTag, Error, behav, ##__VA_ARGS__)
#define LLBC_ErrorAndDoIf4(cond, logger, logTag, behav, ...) LLBC_LogAndDoIf4(cond, logger, logTag, Error, behav, ##__VA_ARGS__)
#define LLBC_FatalAndDoIf(cond, behav, ...) LLBC_LogAndDoIf(cond, Fatal, behav, ##__VA_ARGS__)
#define LLBC_FatalAndDoIf2(cond, logger, behav, ...) LLBC_LogAndDoIf2(cond, logger, Fatal, behav, ##__VA_ARGS__)
#define LLBC_FatalAndDoIf3(cond, logTag, behav, ...) LLBC_LogAndDoIf3(cond, logTag, Fatal, behav, ##__VA_ARGS__)
#define LLBC_FatalAndDoIf4(cond, logger, logTag, behav, ...) LLBC_LogAndDoIf4(cond, logger, logTag, Fatal, behav, ##__VA_ARGS__)
/**
 * <LOG LV> AND CONTINUE IF macros.
 */
#define LLBC_DebugAndContinueIf(cond, ...) LLBC_LogAndContinueIf(cond, Debug, ##__VA_ARGS__)
#define LLBC_DebugAndContinueIf2(cond, logger, ...) LLBC_LogAndContinueIf2(cond, logger, Debug, ##__VA_ARGS__)
#define LLBC_DebugAndContinueIf3(cond, logTag, ...) LLBC_LogAndContinueIf3(cond, logTag, Debug, ##__VA_ARGS__)
#define LLBC_DebugAndContinueIf4(cond, logger, logTag, ...) LLBC_LogAndContinueIf4(cond, logger, logTag, Debug, ##__VA_ARGS__)
#define LLBC_TraceAndContinueIf(cond, ...) LLBC_LogAndContinueIf(cond, Trace, ##__VA_ARGS__)
#define LLBC_TraceAndContinueIf2(cond, logger, ...) LLBC_LogAndContinueIf2(cond, logger, Trace, ##__VA_ARGS__)
#define LLBC_TraceAndContinueIf3(cond, logTag, ...) LLBC_LogAndContinueIf3(cond, logTag, Trace, ##__VA_ARGS__)
#define LLBC_TraceAndContinueIf4(cond, logger, logTag, ...) LLBC_LogAndContinueIf4(cond, logger, logTag, Trace, ##__VA_ARGS__)
#define LLBC_InfoAndContinueIf(cond, ...) LLBC_LogAndContinueIf(cond, Info, ##__VA_ARGS__)
#define LLBC_InfoAndContinueIf2(cond, logger, ...) LLBC_LogAndContinueIf2(cond, logger, Info, ##__VA_ARGS__)
#define LLBC_InfoAndContinueIf3(cond, logTag, ...) LLBC_LogAndContinueIf3(cond, logTag, Info, ##__VA_ARGS__)
#define LLBC_InfoAndContinueIf4(cond, logger, logTag, ...) LLBC_LogAndContinueIf4(cond, logger, logTag, Info, ##__VA_ARGS__)
#define LLBC_WarnAndContinueIf(cond, ...) LLBC_LogAndContinueIf(cond, Warn, ##__VA_ARGS__)
#define LLBC_WarnAndContinueIf2(cond, logger, ...) LLBC_LogAndContinueIf2(cond, logger, Warn, ##__VA_ARGS__)
#define LLBC_WarnAndContinueIf3(cond, logTag, ...) LLBC_LogAndContinueIf3(cond, logTag, Warn, ##__VA_ARGS__)
#define LLBC_WarnAndContinueIf4(cond, logger, logTag, ...) LLBC_LogAndContinueIf4(cond, logger, logTag, Warn, ##__VA_ARGS__)
#define LLBC_ErrorAndContinueIf(cond, ...) LLBC_LogAndContinueIf(cond, Error, ##__VA_ARGS__)
#define LLBC_ErrorAndContinueIf2(cond, logger, ...) LLBC_LogAndContinueIf2(cond, logger, Error, ##__VA_ARGS__)
#define LLBC_ErrorAndContinueIf3(cond, logTag, ...) LLBC_LogAndContinueIf3(cond, logTag, Error, ##__VA_ARGS__)
#define LLBC_ErrorAndContinueIf4(cond, logger, logTag, ...) LLBC_LogAndContinueIf4(cond, logger, logTag, Error, ##__VA_ARGS__)
#define LLBC_FatalAndContinueIf(cond, ...) LLBC_LogAndContinueIf(cond, Fatal, ##__VA_ARGS__)
#define LLBC_FatalAndContinueIf2(cond, logger, ...) LLBC_LogAndContinueIf2(cond, logger, Fatal, ##__VA_ARGS__)
#define LLBC_FatalAndContinueIf3(cond, logTag, ...) LLBC_LogAndContinueIf3(cond, logTag, Fatal, ##__VA_ARGS__)
#define LLBC_FatalAndContinueIf4(cond, logger, logTag, ...) LLBC_LogAndContinueIf4(cond, logger, logTag, Fatal, ##__VA_ARGS__)

/**
 * <LOG LV> AND BREAK IF macros.
 */
#define LLBC_DebugAndBreakIf(cond, ...) LLBC_LogAndBreakIf(cond, Debug, ##__VA_ARGS__)
#define LLBC_DebugAndBreakIf2(cond, logger, ...) LLBC_LogAndBreakIf2(cond, logger, Debug, ##__VA_ARGS__)
#define LLBC_DebugAndBreakIf3(cond, logTag, ...) LLBC_LogAndBreakIf3(cond, logTag, Debug, ##__VA_ARGS__)
#define LLBC_DebugAndBreakIf4(cond, logger, logTag, ...) LLBC_LogAndBreakIf4(cond, logger, logTag, Debug, ##__VA_ARGS__)
#define LLBC_TraceAndBreakIf(cond, ...) LLBC_LogAndBreakIf(cond, Trace, ##__VA_ARGS__)
#define LLBC_TraceAndBreakIf2(cond, logger, ...) LLBC_LogAndBreakIf2(cond, logger, Trace, ##__VA_ARGS__)
#define LLBC_TraceAndBreakIf3(cond, logTag, ...) LLBC_LogAndBreakIf3(cond, logTag, Trace, ##__VA_ARGS__)
#define LLBC_TraceAndBreakIf4(cond, logger, logTag, ...) LLBC_LogAndBreakIf4(cond, logger, logTag, Trace, ##__VA_ARGS__)
#define LLBC_InfoAndBreakIf(cond, ...) LLBC_LogAndBreakIf(cond, Info, ##__VA_ARGS__)
#define LLBC_InfoAndBreakIf2(cond, logger, ...) LLBC_LogAndBreakIf2(cond, logger, Info, ##__VA_ARGS__)
#define LLBC_InfoAndBreakIf3(cond, logTag, ...) LLBC_LogAndBreakIf3(cond, logTag, Info, ##__VA_ARGS__)
#define LLBC_InfoAndBreakIf4(cond, logger, logTag, ...) LLBC_LogAndBreakIf4(cond, logger, logTag, Info, ##__VA_ARGS__)
#define LLBC_WarnAndBreakIf(cond, ...) LLBC_LogAndBreakIf(cond, Warn, ##__VA_ARGS__)
#define LLBC_WarnAndBreakIf2(cond, logger, ...) LLBC_LogAndBreakIf2(cond, logger, Warn, ##__VA_ARGS__)
#define LLBC_WarnAndBreakIf3(cond, logTag, ...) LLBC_LogAndBreakIf3(cond, logTag, Warn, ##__VA_ARGS__)
#define LLBC_WarnAndBreakIf4(cond, logger, logTag, ...) LLBC_LogAndBreakIf4(cond, logger, logTag, Warn, ##__VA_ARGS__)
#define LLBC_ErrorAndBreakIf(cond, ...) LLBC_LogAndBreakIf(cond, Error, ##__VA_ARGS__)
#define LLBC_ErrorAndBreakIf2(cond, logger, ...) LLBC_LogAndBreakIf2(cond, logger, Error, ##__VA_ARGS__)
#define LLBC_ErrorAndBreakIf3(cond, logTag, ...) LLBC_LogAndBreakIf3(cond, logTag, Error, ##__VA_ARGS__)
#define LLBC_ErrorAndBreakIf4(cond, logger, logTag, ...) LLBC_LogAndBreakIf4(cond, logger, logTag, Error, ##__VA_ARGS__)
#define LLBC_FatalAndBreakIf(cond, ...) LLBC_LogAndBreakIf(cond, Fatal, ##__VA_ARGS__)
#define LLBC_FatalAndBreakIf2(cond, logger, ...) LLBC_LogAndBreakIf2(cond, logger, Fatal, ##__VA_ARGS__)
#define LLBC_FatalAndBreakIf3(cond, logTag, ...) LLBC_LogAndBreakIf3(cond, logTag, Fatal, ##__VA_ARGS__)
#define LLBC_FatalAndBreakIf4(cond, logger, logTag, ...) LLBC_LogAndBreakIf4(cond, logger, logTag, Fatal, ##__VA_ARGS__)

/**
 * <LOG LV> AND RETURN IF macros.
 */
#define LLBC_DebugAndReturnIf(cond, ret, ...) LLBC_LogAndReturnIf(cond, Debug, ret, ##__VA_ARGS__)
#define LLBC_DebugAndReturnIf2(cond, logger, ret, ...) LLBC_LogAndReturnIf2(cond, logger, Debug, ret, ##__VA_ARGS__)
#define LLBC_DebugAndReturnIf3(cond, logTag, ret, ...) LLBC_LogAndReturnIf3(cond, logTag, Debug, ret, ##__VA_ARGS__)
#define LLBC_DebugAndReturnIf4(cond, logger, logTag, ret, ...) LLBC_LogAndReturnIf4(cond, logger, logTag, Debug, ret, ##__VA_ARGS__)
#define LLBC_TraceAndReturnIf(cond, ret, ...) LLBC_LogAndReturnIf(cond, Trace, ret, ##__VA_ARGS__)
#define LLBC_TraceAndReturnIf2(cond, logger, ret, ...) LLBC_LogAndReturnIf2(cond, logger, Trace, ret, ##__VA_ARGS__)
#define LLBC_TraceAndReturnIf3(cond, logTag, ret, ...) LLBC_LogAndReturnIf3(cond, logTag, Trace, ret, ##__VA_ARGS__)
#define LLBC_TraceAndReturnIf4(cond, logger, logTag, ret, ...) LLBC_LogAndReturnIf4(cond, logger, logTag, Trace, ret, ##__VA_ARGS__)
#define LLBC_InfoAndReturnIf(cond, ret, ...) LLBC_LogAndReturnIf(cond, Info, ret, ##__VA_ARGS__)
#define LLBC_InfoAndReturnIf2(cond, logger, ret, ...) LLBC_LogAndReturnIf2(cond, logger, Info, ret, ##__VA_ARGS__)
#define LLBC_InfoAndReturnIf3(cond, logTag, ret, ...) LLBC_LogAndReturnIf3(cond, logTag, Info, ret, ##__VA_ARGS__)
#define LLBC_InfoAndReturnIf4(cond, logger, logTag, ret, ...) LLBC_LogAndReturnIf4(cond, logger, logTag, Info, ret, ##__VA_ARGS__)
#define LLBC_WarnAndReturnIf(cond, ret, ...) LLBC_LogAndReturnIf(cond, Warn, ret, ##__VA_ARGS__)
#define LLBC_WarnAndReturnIf2(cond, logger, ret, ...) LLBC_LogAndReturnIf2(cond, logger, Warn, ret, ##__VA_ARGS__)
#define LLBC_WarnAndReturnIf3(cond, logTag, ret, ...) LLBC_LogAndReturnIf3(cond, logTag, Warn, ret, ##__VA_ARGS__)
#define LLBC_WarnAndReturnIf4(cond, logger, logTag, ret, ...) LLBC_LogAndReturnIf4(cond, logger, logTag, Warn, ret, ##__VA_ARGS__)
#define LLBC_ErrorAndReturnIf(cond, ret, ...) LLBC_LogAndReturnIf(cond, Error, ret, ##__VA_ARGS__)
#define LLBC_ErrorAndReturnIf2(cond, logger, ret, ...) LLBC_LogAndReturnIf2(cond, logger, Error, ret, ##__VA_ARGS__)
#define LLBC_ErrorAndReturnIf3(cond, logTag, ret, ...) LLBC_LogAndReturnIf3(cond, logTag, Error, ret, ##__VA_ARGS__)
#define LLBC_ErrorAndReturnIf4(cond, logger, logTag, ret, ...) LLBC_LogAndReturnIf4(cond, logger, logTag, Error, ret, ##__VA_ARGS__)
#define LLBC_FatalAndReturnIf(cond, ret, ...) LLBC_LogAndReturnIf(cond, Fatal, ret, ##__VA_ARGS__)
#define LLBC_FatalAndReturnIf2(cond, logger, ret, ...) LLBC_LogAndReturnIf2(cond, logger, Fatal, ret, ##__VA_ARGS__)
#define LLBC_FatalAndReturnIf3(cond, logTag, ret, ...) LLBC_LogAndReturnIf3(cond, logTag, Fatal, ret, ##__VA_ARGS__)
#define LLBC_FatalAndReturnIf4(cond, logger, logTag, ret, ...) LLBC_LogAndReturnIf4(cond, logger, logTag, Fatal, ret, ##__VA_ARGS__)

/**
 * <LOG LV> AND EXIT IF macros.
 */
#define LLBC_DebugAndExitIf(cond, exitCode, ...) LLBC_LogAndExitIf(cond, Debug, exitCode, ##__VA_ARGS__)
#define LLBC_DebugAndExitIf2(cond, logger, exitCode, ...) LLBC_LogAndExitIf2(cond, logger, Debug, exitCode, ##__VA_ARGS__)
#define LLBC_DebugAndExitIf3(cond, logTag, exitCode, ...) LLBC_LogAndExitIf3(cond, logTag, Debug, exitCode, ##__VA_ARGS__)
#define LLBC_DebugAndExitIf4(cond, logger, logTag, exitCode, ...) LLBC_LogAndExitIf4(cond, logger, logTag, Debug, exitCode, ##__VA_ARGS__)
#define LLBC_TraceAndExitIf(cond, exitCode, ...) LLBC_LogAndExitIf(cond, Trace, exitCode, ##__VA_ARGS__)
#define LLBC_TraceAndExitIf2(cond, logger, exitCode, ...) LLBC_LogAndExitIf2(cond, logger, Trace, exitCode, ##__VA_ARGS__)
#define LLBC_TraceAndExitIf3(cond, logTag, exitCode, ...) LLBC_LogAndExitIf3(cond, logTag, Trace, exitCode, ##__VA_ARGS__)
#define LLBC_TraceAndExitIf4(cond, logger, logTag, exitCode, ...) LLBC_LogAndExitIf4(cond, logger, logTag, Trace, exitCode, ##__VA_ARGS__)
#define LLBC_InfoAndExitIf(cond, exitCode, ...) LLBC_LogAndExitIf(cond, Info, exitCode, ##__VA_ARGS__)
#define LLBC_InfoAndExitIf2(cond, logger, exitCode, ...) LLBC_LogAndExitIf2(cond, logger, Info, exitCode, ##__VA_ARGS__)
#define LLBC_InfoAndExitIf3(cond, logTag, exitCode, ...) LLBC_LogAndExitIf3(cond, logTag, Info, exitCode, ##__VA_ARGS__)
#define LLBC_InfoAndExitIf4(cond, logger, logTag, exitCode, ...) LLBC_LogAndExitIf4(cond, logger, logTag, Info, exitCode, ##__VA_ARGS__)
#define LLBC_WarnAndExitIf(cond, exitCode, ...) LLBC_LogAndExitIf(cond, Warn, exitCode, ##__VA_ARGS__)
#define LLBC_WarnAndExitIf2(cond, logger, exitCode, ...) LLBC_LogAndExitIf2(cond, logger, Warn, exitCode, ##__VA_ARGS__)
#define LLBC_WarnAndExitIf3(cond, logTag, exitCode, ...) LLBC_LogAndExitIf3(cond, logTag, Warn, exitCode, ##__VA_ARGS__)
#define LLBC_WarnAndExitIf4(cond, logger, logTag, exitCode, ...) LLBC_LogAndExitIf4(cond, logger, logTag, Warn, exitCode, ##__VA_ARGS__)
#define LLBC_ErrorAndExitIf(cond, exitCode, ...) LLBC_LogAndExitIf(cond, Error, exitCode, ##__VA_ARGS__)
#define LLBC_ErrorAndExitIf2(cond, logger, exitCode, ...) LLBC_LogAndExitIf2(cond, logger, Error, exitCode, ##__VA_ARGS__)
#define LLBC_ErrorAndExitIf3(cond, logTag, exitCode, ...) LLBC_LogAndExitIf3(cond, logTag, Error, exitCode, ##__VA_ARGS__)
#define LLBC_ErrorAndExitIf4(cond, logger, logTag, exitCode, ...) LLBC_LogAndExitIf4(cond, logger, logTag, Error, exitCode, ##__VA_ARGS__)
#define LLBC_FatalAndExitIf(cond, exitCode, ...) LLBC_LogAndExitIf(cond, Fatal, exitCode, ##__VA_ARGS__)
#define LLBC_FatalAndExitIf2(cond, logger, exitCode, ...) LLBC_LogAndExitIf2(cond, logger, Fatal, exitCode, ##__VA_ARGS__)
#define LLBC_FatalAndExitIf3(cond, logTag, exitCode, ...) LLBC_LogAndExitIf3(cond, logTag, Fatal, exitCode, ##__VA_ARGS__)
#define LLBC_FatalAndExitIf4(cond, logger, logTag, exitCode, ...) LLBC_LogAndExitIf4(cond, logger, logTag, Fatal, exitCode, ##__VA_ARGS__)

#include "llbc/core/log/LoggerMgrInl.h"
