/**
 * @file    ILogAppender.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_ILOG_APPENDER_H__
#define __LLBC_CORE_LOG_ILOG_APPENDER_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_LogTokenChain;
class LLBC_LogRunnable;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * The log appender type class encapsulation.
 */
class LLBC_LogAppenderType
{
public:
    /**
     * The type enumerations.
     */
    enum
    {
        Begin,

        Console = Begin,        // console type appender.
        File,                   // file type appender.
        Network,                // network type appender.

        End
    };
};

/**
 * The log apender init information structure encapsulation.
 */
struct LLBC_LogAppenderInitInfo
{
    LLBC_String pattern;            // output pattern.
    bool colourfulOutput;           // colourful output flag.

    LLBC_String file;               // file name, used File type appender.
    bool forceAppLogPath;           // force use application path to be coming for log file base path.
    bool dailyRolling;              // daily rolling mode flag, used in File type appender.
    long maxFileSize;               // max log file size, int bytes, used in File type appender.
    int maxBackupIndex;             // max backup index, used in File type appender.
    int fileBufferSize;             // file buffer size, used in File type appender.

    LLBC_String ip;                 // Ip address, used in Network type appender.
    uint16 port;                    // port, used in Network type appender.
};

/**
 * \brief The log appender interface class encapsulation.
 */
class LLBC_ILogAppender
{
public:
    virtual ~LLBC_ILogAppender() {  }

public:
    /**
     * Get log appender type, see LLBC_LogAppenderType.
     * @return int - log appender type.
     */
    virtual int GetType() const = 0;

public:
    /**
     * Initialize the log appender.
     * @param[in] initInfo - log appender initialize info structure.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Initialize(const LLBC_LogAppenderInitInfo &initInfo) = 0;

    /**
     * Finalize the appender.
     */
    virtual void Finalize() = 0;

    /**
     * Output log data.
     * @param[in] data - log data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Output(const LLBC_LogData &data) = 0;

protected:
    /**
     * Get current appender's token chain.
     * @return LLBC_LogTokenChain * - the log token chain.
     */
    virtual LLBC_LogTokenChain *GetTokenChain() const = 0;

protected:
    friend class LLBC_LogRunnable;

    /**
     * Get next appender.
     * @return LLBC_ILogAppender * - next log appender.
     */
    virtual LLBC_ILogAppender *GetAppenderNext() const = 0;

    /**
     * Set next appender.
     * @param[in] appender - next log appender.
     */
    virtual void SetAppenderNext(LLBC_ILogAppender *appender) = 0;

protected:
    /**
     * Flush method.
     */
    virtual void Flush() = 0;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_ILOG_APPENDER_H__
