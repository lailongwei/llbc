/**
 * @file    LogNetworkAppender.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_NETWORK_APPENDER_H__
#define __LLBC_CORE_LOG_LOG_NETWORK_APPENDER_H__

#include "llbc/common/Common.h"

#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

/**
 * \brief Network log appender class encapsulation.
 */
class LLBC_LogNetworkAppender : public LLBC_BaseLogAppender
{
    typedef LLBC_BaseLogAppender _Base;

public:
    LLBC_LogNetworkAppender();
    virtual ~LLBC_LogNetworkAppender();

public:
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
    LLBC_String _ip;
    uint16 _port;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_NETWORK_APPENDER_H__
