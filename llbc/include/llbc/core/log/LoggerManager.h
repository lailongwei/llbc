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
#endif

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Logger;
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

private:
    LLBC_RecursiveLock _lock;

    LLBC_Logger *_root;
    std::map<LLBC_String, LLBC_Logger *> _loggers;

    LLBC_LoggerConfigurator *_configurator;

    static LLBC_String _rootLoggerName;

    LLBC_LogRunnable *_logRunnable;
};

/**
 * Singleton class macro define.
 */
template class LLBC_EXPORT LLBC_Singleton<LLBC_LoggerManager>;
#define LLBC_LoggerManagerSingleton LLBC_Singleton<LLBC_NS LLBC_LoggerManager>::Instance()

/**
 * root logger operations macro define.
 */
#if LLBC_CFG_LOG_USING_WITH_STREAM

#define LLBC_DEBUG_LOG(message)                                                        \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Debug(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());     \
    } while(0)                                                                         \

#define LLBC_INFO_LOG(message)                                                         \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Info(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());      \
    } while(0)                                                                         \

#define LLBC_WARN_LOG(message)                                                         \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Warn(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());      \
    } while(0)                                                                         \

#define LLBC_ERROR_LOG(message)                                                        \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Error(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());     \
    } while(0)                                                                         \

#define LLBC_FATAL_LOG(message)                                                        \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Fatal(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());     \
    } while(0)                                                                         \

#define LLBC_DEBUG_LOG2(tag, message)                                                  \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Debug(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());      \
    } while(0)                                                                         \

#define LLBC_INFO_LOG2(tag, message)                                                   \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Info(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());       \
    } while(0)                                                                         \

#define LLBC_WARN_LOG2(tag, message)                                                   \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Warn(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());       \
    } while(0)                                                                         \

#define LLBC_ERROR_LOG2(tag, message)                                                  \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Error(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());      \
    } while(0)                                                                         \

#define LLBC_FATAL_LOG2(tag, message)                                                  \
    do {                                                                               \
        LLBC_NS LLBC_LogMessageBuffer oss;                                             \
        LLBC_NS LLBC_Logger *l = LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger(); \
        l->Fatal(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());      \
    } while(0)                                                                         \

#else // !LLBC_CFG_LOG_USING_WITH_STREAM

#define LLBC_DEBUG_LOG(fmt, ...)                            \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Debug(NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__) \

#define LLBC_INFO_LOG(fmt, ...)                             \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Info(NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)  \

#define LLBC_WARN_LOG(fmt, ...)                             \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Warn(NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)  \

#define LLBC_ERROR_LOG(fmt, ...)                            \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Error(NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__) \

#define LLBC_FATAL_LOG(fmt, ...)                            \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Fatal(NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__) \

#define LLBC_DEBUG_LOG2(tag, fmt, ...)                      \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Debug(tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)  \

#define LLBC_INFO_LOG2(tag, fmt, ...)                       \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Info(tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)   \

#define LLBC_WARN_LOG2(tag, fmt, ...)                       \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Warn(tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)   \

#define LLBC_ERROR_LOG2(tag, fmt, ...)                      \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Error(tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)  \

#define LLBC_FATAL_LOG2(tag, fmt, ...)                      \
    LLBC_NS LLBC_LoggerManagerSingleton->GetRootLogger()->  \
        Fatal(tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)  \

#endif // LLBC_CFG_LOG_USING_WITH_STREAM

/**
 * Specific logger operations macro.
 */
#if LLBC_CFG_LOG_USING_WITH_STREAM

#define LLBC_DEBUG_LOG_SPEC(logger, message)                                         \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Debug(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());   \
    } while(0)                                                                       \

#define LLBC_INFO_LOG_SPEC(logger, message)                                          \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Info(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());    \
    } while(0)                                                                       \

#define LLBC_WARN_LOG_SPEC(logger, message)                                          \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Warn(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());    \
    } while(0)                                                                       \

#define LLBC_ERROR_LOG_SPEC(logger, message)                                         \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Error(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());   \
    } while(0)                                                                       \

#define LLBC_FATAL_LOG_SPEC(logger, message)                                         \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Fatal(NULL, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());   \
    } while(0)                                                                       \

#define LLBC_DEBUG_LOG_SPEC2(logger, tag, message)                                   \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Debug(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());    \
    } while(0)                                                                       \

#define LLBC_INFO_LOG_SPEC2(logger, tag, message)                                    \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Info(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());     \
    } while(0)                                                                       \

#define LLBC_WARN_LOG_SPEC2(logger, tag, message)                                    \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Warn(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());     \
    } while(0)                                                                       \

#define LLBC_ERROR_LOG_SPEC2(logger, tag, message)                                   \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Error(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());    \
    } while(0)                                                                       \

#define LLBC_FATAL_LOG_SPEC2(logger, tag, message)                                   \
    do {                                                                             \
        LLBC_NS LLBC_LogMessageBuffer oss;                                           \
        LLBC_NS LLBC_Logger *l = LLBC_LoggerManagerSingleton->GetLogger(logger);     \
        l->Fatal(tag, __FILE__, __LINE__, "%s", oss.str(oss << message).c_str());    \
    } while(0)                                                                       \

#else // !LLBC_CFG_LOG_USING_WITH_STREAM

#define LLBC_DEBUG_LOG_SPEC(logger, message, ...)               \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Debug(NULL, __FILE__, __LINE__, message, ##__VA_ARGS__) \

#define LLBC_INFO_LOG_SPEC(logger, message, ...)                \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Info(NULL, __FILE__, __LINE__, message, ##__VA_ARGS__)  \

#define LLBC_WARN_LOG_SPEC(logger, message, ...)                \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Warn(NULL, __FILE__, __LINE__, message, ##__VA_ARGS__)  \

#define LLBC_ERROR_LOG_SPEC(logger, message, ...)               \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Error(NULL, __FILE__, __LINE__, message, ##__VA_ARGS__) \

#define LLBC_FATAL_LOG_SPEC(logger, message, ...)               \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Fatal(NULL, __FILE__, __LINE__, message, ##__VA_ARGS__) \

#define LLBC_DEBUG_LOG_SPEC2(logger, tag, message, ...)         \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Debug(tag, __FILE__, __LINE__, message, ##__VA_ARGS__)  \

#define LLBC_INFO_LOG_SPEC2(logger, tag, message, ...)          \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Info(tag, __FILE__, __LINE__, message, ##__VA_ARGS__)   \

#define LLBC_WARN_LOG_SPEC2(logger, tag, message, ...)          \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Warn(tag, __FILE__, __LINE__, message, ##__VA_ARGS__)   \

#define LLBC_ERROR_LOG_SPEC2(logger, tag, message, ...)         \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Error(tag, __FILE__, __LINE__, message, ##__VA_ARGS__)  \

#define LLBC_FATAL_LOG_SPEC2(logger, tag, message, ...)         \
    LLBC_NS LLBC_LoggerManagerSingleton->GetLogger(logger)->    \
        Fatal(tag, __FILE__, __LINE__, message, ##__VA_ARGS__)  \

#endif // LLBC_CFG_LOG_USING_WITH_STREAM

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOGGER_MANAGER_H__
