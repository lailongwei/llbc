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

#ifndef __LLBC_CORE_LOG_LOGGER_MANAGER_H__
#define __LLBC_CORE_LOG_LOGGER_MANAGER_H__

#include "llbc/common/Common.h"

#include "llbc/core/singleton/Singleton.h"
#include "llbc/core/thread/RecursiveLock.h"

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
class LLBC_EXPORT LLBC_LoggerManager
{
public:
    LLBC_LoggerManager();
    ~LLBC_LoggerManager();

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
    LLBC_Logger *GetLogger(const LLBC_String &name) const;

    /**
     * Uninit output.
     * @param logLv - the log level.
     * @param msg   - the log message.
     */
    void UnInitOutput(int logLv, const char *msg);

private:
    mutable LLBC_RecursiveLock _lock;

    LLBC_LoggerConfigurator *_configurator;
    LLBC_LogRunnable *_sharedLogRunnable;

    LLBC_Logger * volatile _rootLogger;
    std::map<LLBC_String, LLBC_Logger *> _loggers;

    static LLBC_String _rootLoggerName;
};

/**
 * Singleton class macro define.
 */
template class LLBC_EXPORT LLBC_Singleton<LLBC_LoggerManager>;
#define LLBC_LoggerManagerSingleton LLBC_Singleton<LLBC_NS LLBC_LoggerManager>::Instance()

/**
 * @brief The llbc library log macro define.
 * @param[in] loggerName - the logger name, nullptr if log to root.
 * @param[in] tag        - the log tag, nullptr if no tag.
 * @param[in] level      - log level.
 * @param[in] fmt        - the log message format string.
 * @param[in] ...        - the variadic log message parameters.
 */
#define LLOG(loggerName, tag, level, fmt, ...)                            \
    {                                                                     \
        if (UNLIKELY(!LLBC_NS LLBC_LoggerManagerSingleton->IsInited())) { \
            LLBC_NS __LLBC_LibTls *libTls = LLBC_NS __LLBC_GetLibTls();   \
            int len = ::snprintf(libTls->coreTls.loggerFmtBuf, sizeof(libTls), fmt, ##__VA_ARGS__); \
            if (len >= 0)                                                 \
                LLBC_LoggerManagerSingleton->UnInitOutput(                \
                    LLBC_NS LLBC_LogLevel::level, libTls->coreTls.loggerFmtBuf); \
        }                                                                 \
        else                                                              \
        {                                                                 \
            LLBC_NS LLBC_Logger *l;                                       \
            if (loggerName == nullptr)                                    \
                l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
            else                                                          \
                l = LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(loggerName); \
                                                                          \
            if (LIKELY(l))                                                \
                l->Output(LLBC_NS LLBC_LogLevel::level,                   \
                          tag,                                            \
                          __FILE__,                                       \
                          __LINE__,                                       \
                          __FUNCTION__,                                   \
                          fmt,                                            \
                          ##__VA_ARGS__);                                 \
        }                                                                 \
    }                                                                     \

#define LLOG_DEBUG(fmt, ...) LLOG(nullptr, nullptr, Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG2(tag, fmt, ...) LLOG(nullptr, tag, Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG3(loggerName, fmt, ...) LLOG(loggerName, nullptr, Debug, fmt, ##__VA_ARGS__)
#define LLOG_DEBUG4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, Debug, fmt, ##__VA_ARGS__)

#define LLOG_INFO(fmt, ...) LLOG(nullptr, nullptr, Info, fmt, ##__VA_ARGS__)
#define LLOG_INFO2(tag, fmt, ...) LLOG(nullptr, tag, Info, fmt, ##__VA_ARGS__)
#define LLOG_INFO3(loggerName, fmt, ...) LLOG(loggerName, nullptr, Info, fmt, ##__VA_ARGS__)
#define LLOG_INFO4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, Info, fmt, ##__VA_ARGS__)

#define LLOG_WARN(fmt, ...) LLOG(nullptr, nullptr, Warn, fmt, ##__VA_ARGS__)
#define LLOG_WARN2(tag, fmt, ...) LLOG(nullptr, tag, Warn, fmt, ##__VA_ARGS__)
#define LLOG_WARN3(loggerName, fmt, ...) LLOG(loggerName, nullptr, Warn, fmt, ##__VA_ARGS__)
#define LLOG_WARN4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, Warn, fmt, ##__VA_ARGS__)

#define LLOG_ERROR(fmt, ...) LLOG(nullptr, nullptr, Error, fmt, ##__VA_ARGS__)
#define LLOG_ERROR2(tag, fmt, ...) LLOG(nullptr, tag, Error, fmt, ##__VA_ARGS__)
#define LLOG_ERROR3(loggerName, fmt, ...) LLOG(loggerName, nullptr, Error, fmt, ##__VA_ARGS__)
#define LLOG_ERROR4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, Error, fmt, ##__VA_ARGS__)

#define LLOG_FATAL(fmt, ...) LLOG(nullptr, nullptr, Fatal, fmt, ##__VA_ARGS__)
#define LLOG_FATAL2(tag, fmt, ...) LLOG(nullptr, tag, Fatal, fmt, ##__VA_ARGS__)
#define LLOG_FATAL3(loggerName, fmt, ...) LLOG(loggerName, nullptr, Fatal, fmt, ##__VA_ARGS__)
#define LLOG_FATAL4(loggerName, tag, fmt, ...) LLOG(loggerName, tag, Fatal, fmt, ##__VA_ARGS__)

/**
 * The llbc library json log macro define.
 * @param[in] loggerName - the logger name, nullptr if log to root.
 * @param[in] tag        - the log tag, nullptr if no tag.
 * @param[in] level      - log level.
 */
#define LJLOG(loggerName, tag, level)                    \
    (*LLBC_New(LLBC_NS LLBC_LogJsonMsg, loggerName, tag, LLBC_NS LLBC_LogLevel::level, __FILE__, __LINE__, __FUNCTION__)) \

#define LJLOG_DEBUG() LJLOG(nullptr, nullptr, Debug)
#define LJLOG_DEBUG2(tag) LJLOG(nullptr, tag, Debug)
#define LJLOG_DEBUG3(loggerName) LJLOG(loggerName, nullptr, Debug)
#define LJLOG_DEBUG4(loggerName, tag) LJLOG(loggerName, tag, Debug)

#define LJLOG_INFO() LJLOG(nullptr, nullptr, Info)
#define LJLOG_INFO2(tag) LJLOG(nullptr, tag, Info)
#define LJLOG_INFO3(loggerName) LJLOG(loggerName, nullptr, Info)
#define LJLOG_INFO4(loggerName, tag) LJLOG(loggerName, tag, Info)

#define LJLOG_WARN() LJLOG(nullptr, nullptr, Warn)
#define LJLOG_WARN2(tag) LJLOG(nullptr, tag, Warn)
#define LJLOG_WARN3(loggerName) LJLOG(loggerName, nullptr, Warn)
#define LJLOG_WARN4(loggerName, tag) LJLOG(loggerName, tag, Warn)

#define LJLOG_ERROR() LJLOG(nullptr, nullptr, Error) 
#define LJLOG_ERROR2(tag) LJLOG(nullptr, tag, Error)
#define LJLOG_ERROR3(loggerName) LJLOG(loggerName, nullptr, Error)
#define LJLOG_ERROR4(loggerName, tag) LJLOG(loggerName, tag, Error)

#define LJLOG_FATAL() LJLOG(nullptr, nullptr, Fatal)
#define LJLOG_FATAL2(tag) LJLOG(nullptr, tag, Fatal)
#define LJLOG_FATAL3(loggerName) LJLOG(loggerName, nullptr, Fatal)
#define LJLOG_FATAL4(loggerName, tag) LJLOG(loggerName, tag, Fatal)

#if LLBC_CFG_LOG_USING_WITH_STREAM
/**
 * The llbc library stream log macro define.
 * @param[in] loggerName - the logger name, nullptr if log to root.
 * @param[in] tag        - the log tag, nullptr if no tag.
 * @param[in] level      - log level.
 * @param[in] streamMsg  - stream message, like: '"the log message, intVal:" << 3 <<", strVal:" << "strMsg"'
 */
#define LSLOG(loggerName, tag, level, streamMsg)                          \
    {                                                                     \
        if (UNLIKELY(!LLBC_LoggerManagerSingleton->IsInited())) {         \
            LLBC_NS LLBC_LogMessageBuffer lmb;                            \
            LLBC_LoggerManagerSingleton->UnInitOutput(                    \
                LLBC_NS LLBC_LogLevel::level, lmb.str(lmb << streamMsg).c_str()); \
        }                                                                 \
        else {                                                            \
            LLBC_NS LLBC_Logger *l;                                       \
            if (loggerName == nullptr)                                    \
                l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
            else                                                          \
                l = LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(loggerName); \
                                                                          \
            if (LIKELY(l)) {                                              \
                LLBC_NS LLBC_LogMessageBuffer lmb;                        \
                const LLBC_String &formattedLogMsg = lmb.str(lmb << streamMsg); \
                l->NonFormatOutput(LLBC_NS LLBC_LogLevel::level,          \
                                   tag,                                   \
                                   __FILE__,                              \
                                   __LINE__,                              \
                                   __FUNCTION__,                          \
                                   formattedLogMsg.c_str(),               \
                                   formattedLogMsg.length());             \
            }                                                             \
        }                                                                 \
    }                                                                     \

#define LSLOG_DEBUG(streamMsg) LSLOG(nullptr, nullptr, Debug, streamMsg)
#define LSLOG_DEBUG2(tag, streamMsg) LSLOG(nullptr, tag, Debug, streamMsg)
#define LSLOG_DEBUG3(loggerName, streamMsg) LSLOG(loggerName, nullptr, Debug, streamMsg
#define LSLOG_DEBUG4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, Debug, streamMsg)

#define LSLOG_INFO(streamMsg) LSLOG(nullptr, nullptr, Info, streamMsg)
#define LSLOG_INFO2(tag, streamMsg) LSLOG(nullptr, tag, Info, streamMsg)
#define LSLOG_INFO3(loggerName, streamMsg) LSLOG(loggerName, nullptr, Info, streamMsg)
#define LSLOG_INFO4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, Info, streamMsg)

#define LSLOG_WARN(streamMsg) LSLOG(nullptr, nullptr, Warn, streamMsg)
#define LSLOG_WARN2(tag, streamMsg) LSLOG(nullptr, tag, Warn, streamMsg)
#define LSLOG_WARN3(loggerName, streamMsg) LSLOG(loggerName, nullptr, Warn, streamMsg)
#define LSLOG_WARN4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, Warn, streamMsg)

#define LSLOG_ERROR(streamMsg) LSLOG(nullptr, nullptr, Error, streamMsg)
#define LSLOG_ERROR2(tag, streamMsg) LSLOG(nullptr, tag, Error, streamMsg)
#define LSLOG_ERROR3(loggerName, streamMsg) LSLOG(loggerName, nullptr, Error, streamMsg
#define LSLOG_ERROR4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, Error, streamMsg)

#define LSLOG_FATAL(streamMsg) LSLOG(nullptr, nullptr, Fatal, streamMsg)
#define LSLOG_FATAL2(tag, streamMsg) LSLOG(nullptr, tag, Fatal, streamMsg)
#define LSLOG_FATAL3(loggerName, streamMsg) LSLOG(loggerName, nullptr, Fatal, streamMsg)
#define LSLOG_FATAL4(loggerName, tag, streamMsg) LSLOG(loggerName, tag, Fatal, streamMsg)

#endif // LLBC_CFG_LOG_USING_WITH_STREAM

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOGGER_MANAGER_H__
