/**
 * @file    LogFileAppender.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_FILE_APPENDER_H__
#define __LLBC_CORE_LOG_LOG_FILE_APPENDER_H__

#include "llbc/common/Common.h"

#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_File;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief File log appender class encapsulation.
 */
class LLBC_LogFileAppender : public LLBC_BaseLogAppender
{
    typedef LLBC_BaseLogAppender _Base;

public:
    LLBC_LogFileAppender();
    virtual ~LLBC_LogFileAppender();

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
     * Get log file.
     * @return LLBC_String - log file name.
     */
    int OpenLogFile(LLBC_File &file) const;

private:
    bool _isDailyRolling;

    size_t _maxFileSize;
    int _maxBackupIndex;

    LLBC_String _fileName;

    LLBC_File *_file;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_FILE_APPENDER_H__
