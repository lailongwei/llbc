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
#include "llbc/core/thread/DummyLock.h"
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
class LLBC_LoggerConfigurator;
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
     * @param[in] cfgFile - config file.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &cfgFile);

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
                               

private:
    mutable LLBC_DummyLock _lock;

    LLBC_LoggerConfigurator *_configurator;
    LLBC_LogRunnable *_sharedLogRunnable;

    LLBC_Logger * volatile _rootLogger;
    std::map<LLBC_String, LLBC_Logger *> _str2Loggers;
    std::map<LLBC_CString, LLBC_Logger *> _cstr2Loggers;
    std::vector<std::pair<LLBC_CString, LLBC_Logger *> > _loggerList;
    std::map<LLBC_String, LLBC_Logger *>::const_iterator _str2LoggersEnd;
    std::map<LLBC_CString, LLBC_Logger *>::const_iterator _cstr2LoggersEnd;

    static LLBC_String _rootLoggerName;
};

/**
 * Singleton class macro define.
 */
template class LLBC_EXPORT LLBC_Singleton<LLBC_LoggerMgr>;
#define LLBC_LoggerMgrSingleton LLBC_NS LLBC_Singleton<LLBC_NS LLBC_LoggerMgr>::Instance()

/**
 * @brief The llbc library log macro define.
 * @param[in] loggerName - the logger name, nullptr if log to root.
 * @param[in] tag        - the log tag, nullptr if no tag.
 * @param[in] level      - log level.
 * @param[in] fmt        - the log message format string.
 * @param[in] ...        - the variadic log message parameters.
 */
#define LLOG(loggerName, tag, level, fmt, ...)            \
    do {                                                  \
        auto __loggerMgr__ = LLBC_LoggerMgrSingleton;     \
        if (LIKELY(__loggerMgr__->IsInited())) {          \
            LLBC_NS LLBC_Logger *__l__;                   \
            if (loggerName != nullptr) {                  \
                __l__ = __loggerMgr__->GetLogger(loggerName); \
                if (UNLIKELY(__l__ == nullptr))           \
                    break;                                \
            }                                             \
            else {                                        \
                __l__ = __loggerMgr__->GetRootLogger();   \
            }                                             \
                                                          \
            if (level < __l__->GetLogLevel())             \
                break;                                    \
                                                          \
            __l__->Output(level,                          \
                          tag,                            \
                          __FILE__,                       \
                          __LINE__,                       \
                          __FUNCTION__,                   \
                          fmt,                            \
                          ##__VA_ARGS__);                 \
        }                                                 \
        else {                                            \
            __loggerMgr__->UnInitOutput(level,            \
                                        tag,              \
                                        __FILE__,         \
                                        __LINE__,         \
                                        __FUNCTION__,     \
                                        fmt,              \
                                        ##__VA_ARGS__);   \
        }                                                 \
    } while (false)                                       \

#define LLOG_TRACE(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)
#define LLOG_TRACE2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)
#define LLOG_TRACE3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)
#define LLOG_TRACE4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Trace, fmt, ##__VA_ARGS__)

#define LLOG_DEBUG(fmt, ...) LLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG2(loggerName, fmt, ...) LLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG3(tag, fmt, ...) LLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Debug, fmt, ##__VA_ARGS__)

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
        (*(new LLBC_NS LLBC_LogJsonMsg(                  \
                   LIKELY(LLBC_LoggerMgrSingleton->IsInited()) ? \
                        (loggerName != nullptr ?         \
                            LLBC_LoggerMgrSingleton->GetLogger(loggerName) : \
                                LLBC_LoggerMgrSingleton->GetRootLogger()) : nullptr, \
                   tag,                                  \
                   level,                                \
                   __FILE__,                             \
                   __LINE__,                             \
                   __FUNCTION__)))                       \

#define LJLOG_TRACE() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Trace)
#define LJLOG_TRACE2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Trace)
#define LJLOG_TRACE3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Trace)
#define LJLOG_TRACE4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Trace)

#define LJLOG_DEBUG() LJLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Debug)
#define LJLOG_DEBUG2(loggerName) LJLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Debug)
#define LJLOG_DEBUG3(tag) LJLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Debug)
#define LJLOG_DEBUG4(loggerName, tag) LJLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Debug)

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
 * Log operator for condition judge helper macros.
 */
template<int>
class __LLBC_ConditionLogOperator
{
public:
    static void Output(const char *fileName, int lineNo, const char *funcName, int logLv,
                       const char *fmt1, const char *cond, const char *behav,
                       const char *fmt2, ...) LLBC_STRING_FORMAT_CHECK(8, 9);
};

template<int ARG_COUNT>
void __LLBC_ConditionLogOperator<ARG_COUNT>::Output(const char *fileName, int lineNo, const char *funcName,
                                                    int logLv, const char *fmt1, const char *cond,
                                                    const char *behav, const char *fmt2, ...)
{

    va_list ap;
    va_start(ap, fmt2);

    __LLBC_LibTls *libTls = __LLBC_GetLibTls();

    // Format format 1 and format 2.
    int fmt1Len = snprintf(libTls->coreTls.loggerFmtBuf,
                           sizeof libTls->coreTls.loggerFmtBuf,
                           fmt1, cond, behav);
    int fmt2Len = vsnprintf(libTls->coreTls.loggerFmtBuf + fmt1Len,
                            sizeof libTls->coreTls.loggerFmtBuf - fmt1Len,
                            fmt2, ap);
    va_end(ap);
    
    LLBC_ReturnIf(fmt2Len <= 0, void());
    
    auto* loggerMgr = LLBC_LoggerMgrSingleton;
    if (LIKELY(loggerMgr->IsInited()))
    {
        LLBC_Logger *logger = loggerMgr->GetRootLogger();
        LLBC_ReturnIf(logLv < logger->GetLogLevel(), void());

        logger->NonFormatOutput(logLv, nullptr, fileName, lineNo, funcName, LLBC_GetMicroSeconds(),
                                libTls->coreTls.loggerFmtBuf, fmt1Len + fmt2Len);
    }
    else
    {
        loggerMgr->UnInitNonFormatOutput(logLv, nullptr, fileName, lineNo, funcName,
                                         libTls->coreTls.loggerFmtBuf, fmt1Len + fmt2Len);
    }
}

template<>
class __LLBC_ConditionLogOperator<0>
{
public:
    static void Output(const char *fileName, int lineNo, const char *funcName, int logLv, const char *fmt,
                       const char *cond, const char *behav)
    {
        auto *loggerMgr = LLBC_LoggerMgrSingleton;     
        if (LIKELY(loggerMgr->IsInited()))
        {
            LLBC_Logger *logger = loggerMgr->GetRootLogger();   
            if (logger->GetLogLevel() <= logLv)
                logger->Output(logLv, nullptr, fileName, lineNo, funcName, fmt, cond, behav);
        }
        else
        {
            loggerMgr->UnInitOutput(logLv, nullptr, fileName, lineNo, funcName, fmt, cond, behav);
        }
    }
};


/**
 * Some condition judge helper with log print macros.
 */
#define LLBC_LogAndDoIf(cond, logLv, behav, ...)                                                                \
    do {                                                                                                        \
        if (cond) {                                                                                             \
            __LLBC_ConditionLogOperator<std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output(\
                __FILE__, __LINE__, __FUNCTION__,                                                               \
                LLBC_NS LLBC_LogLevel::logLv, "LLBC_DoIf:<\"%s\"> is true, do:%s. ",                            \
                #cond, #behav, ##__VA_ARGS__);                                                              \
            behav;                                                                                              \
        }                                                                                                       \
    } while(false)

#define LLBC_LogAndContinueIf(cond, logLv, ...)                                                                 \
    do {                                                                                                        \
        if (cond) {                                                                                             \
            __LLBC_ConditionLogOperator<std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output(\
                __FILE__, __LINE__, __FUNCTION__,                                                               \
                LLBC_NS LLBC_LogLevel::logLv, "LLBC_ContinueIf:<\"%s\"> is true. %s",                           \
                #cond, "", ##__VA_ARGS__);                                                                  \
            continue;                                                                                           \
        }                                                                                                       \
    } while(false)

#define LLBC_LogAndReturnIf(cond, logLv, ret, ...)                                                              \
    do {                                                                                                        \
        if (cond) {                                                                                             \
            __LLBC_ConditionLogOperator<std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output(\
                __FILE__, __LINE__, __FUNCTION__,                                                               \
                LLBC_NS LLBC_LogLevel::logLv, "LLBC_ReturnIf:<\"%s\"> is true, return:%s. ",                    \
                #cond, #ret, ##__VA_ARGS__);                                                                \
            return ret;                                                                                         \
        }                                                                                                       \
    } while(false) 

#define LLBC_LogAndBreakIf(cond, logLv, ...)                                                                    \
    if (cond) {                                                                                                 \
        __LLBC_ConditionLogOperator<std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::Output(    \
            __FILE__, __LINE__, __FUNCTION__,                                                                       \
            LLBC_NS LLBC_LogLevel::logLv, "LLBC_LogAndBreakIf:<\"%s\"> is true. %s",                            \
            #cond, "", ##__VA_ARGS__);                                                                      \
        break;                                                                                                  \
    }                                                                                                           \

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

#define LSLOG_TRACE(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Trace, streamMsg)
#define LSLOG_TRACE2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Trace, streamMsg)
#define LSLOG_TRACE3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Trace, streamMsg)
#define LSLOG_TRACE4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Trace, streamMsg)

#define LSLOG_DEBUG(streamMsg) LSLOG(nullptr, nullptr, LLBC_NS LLBC_LogLevel::Debug, streamMsg)
#define LSLOG_DEBUG2(loggerName, streamMsg) LSLOG(loggerName, nullptr, LLBC_NS LLBC_LogLevel::Debug, streamMsg)
#define LSLOG_DEBUG3(tag, streamMsg) LSLOG(nullptr, tag, LLBC_NS LLBC_LogLevel::Debug, streamMsg)
#define LSLOG_DEBUG4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, LLBC_NS LLBC_LogLevel::Debug, streamMsg)

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

__LLBC_NS_END

#include "llbc/core/log/LoggerMgrInl.h"


