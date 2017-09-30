/**
 * @file    LogConsoleAppender.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_CONSOLE_APPENDER_H__
#define __LLBC_CORE_LOG_LOG_CONSOLE_APPENDER_H__

#include "llbc/common/Common.h"

#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

/**
 * \brief The console type log appender class encapsulation.
 */
class LLBC_LogConsoleAppender : public LLBC_BaseLogAppender
{
    typedef LLBC_BaseLogAppender _Base;

public:
    LLBC_LogConsoleAppender();
    virtual ~LLBC_LogConsoleAppender();

public:
    /**
     * Get log appender type, see LLBC_LogAppenderType.
     * @return int - log appender type.
     */
    virtual int GetType() const;

public:
    /**
     * Initialize the log appender.
     * @param[in] initInfo - log appender initialize info structure.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Initialize(const LLBC_LogAppenderInitInfo &initInfo);

    /**
     * Finalize the appender.
     */
    virtual void Finalize();

    /**
     * Output log data.
     * @param[in] data - log data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Output(const LLBC_LogData &data);

private:
    /**
     * Determine the log text output color, only available in WIN platform.
     * @param[in] logLv - the log level.
     * @return int - the output color.
     */
    int DetermineLogTextColor(int logLv);

	/**
     * Determine the log text output color, only available in Unix-like platform.
     * @param[in] logLv - the log level.
     * @return const char* - the output color.
     */
	const char* DetermineAnsiTextColor(int logLv);

private:
    bool _colourfulOutput;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_CONSOLE_APPENDER_H__
