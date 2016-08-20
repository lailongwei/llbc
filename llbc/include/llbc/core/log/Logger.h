/**
 * @file    Logger.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOGGER_H__
#define __LLBC_CORE_LOG_LOGGER_H__

#include "llbc/common/Common.h"

#include "llbc/core/thread/RecursiveLock.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_LogRunnable;
class LLBC_LoggerConfigInfo;

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
     * @param[in] name   - logger name.
     * @param[in] config - logger config info.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Initialize(const LLBC_String &name, const LLBC_LoggerConfigInfo *config);

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
     * Set log level.
     * @param[in] level - new log level.
     */
    void SetLogLevel(int level);

public:
    /**
     * Output specific level message.
     * @param[in] tag      - log tag, can set to NULL.
     * @param[in] file     - log file name.
     * @param[in] line     - log file line.
     * @param[in] message  - format control string.
     * @param[in] argument - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Debug(const char *tag, const char *file, int line, const char *message, ...);
    int Info(const char *tag, const char *file, int line, const char *message, ...);
    int Warn(const char *tag, const char *file, int line, const char *message, ...);
    int Error(const char *tag, const char *file, int line, const char *message, ...);
    int Fatal(const char *tag, const char *file, int line, const char *message, ...);

    /**
     * Output message using given level.
     * @param[in] level    - log level.
     * @param[in] tag      - log tag, can set to NULL.
     * @param[in] file     - log file name.
     * @param[in] line     - log file line.
     * @param[in] message  - format control string.
     * @param[in] argument - optional arguments.
     * @return int - return 0 if success, otherwise return -1.
     */
    int Output(int level, const char *tag, const char *file, int line, const char *message, ...);

private:
    /**
     * Direct output message using given level.
     */
    int DirectOutput(int level, const char *tag, const char *file, int line, char *message, int len);
    /**
     * Build log data.
     * @param[in] level   - log level.
     * @param[in] tag     - log tag.
     * @param[in] file    - log file name.
     * @param[in] line    - log file line.
     * @param[in] message - log format control string.
     * @param[in] len     - the message length, not include tailing character.
     * @return LLBC_LogData * - the log data.
     */
    LLBC_LogData *BuildLogData(int level,
                               const char *tag,
                               const char *file,
                               int line,
                               char *message,
                               int len);

private:
    LLBC_RecursiveLock _mutex;

    LLBC_String _name;

    int _logLevel;
    const LLBC_LoggerConfigInfo *_config;

    LLBC_LogRunnable *_logRunnable;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOGGER_H__
