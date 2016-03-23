/**
 * @file    ProcessIdToken.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/03/23
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_PROCESS_ID_TOKEN_H__
#define __LLBC_CORE_LOG_LOG_PROCESS_ID_TOKEN_H__

#include "llbc/common/Common.h"
#include "llbc/core/log/BaseLogToken.h"

__LLBC_NS_BEGIN

/**
 * \brief The logger process Id token class encapsulation.
 */
class LLBC_HIDDEN LLBC_LogProcessIdToken : public LLBC_BaseLogToken
{
public:
    LLBC_LogProcessIdToken();
    virtual ~LLBC_LogProcessIdToken();

public:
    /**
     * Initialize the log token.
     * @param[in] formatter - log formatter.
     * @param[in] str       - token append string data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str);

    /**
     * Get token type.
     * @return int - token type.
     */
    virtual int GetType() const;

    /**
     * Format the log data.
     * @param[in] data           - log data.
     * @param[out] formattedData - store location for formatted log string.
     */
    virtual void Format(const LLBC_LogData &data, LLBC_String &formattedData) const;

private:
    int _processId;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_PROCESS_ID_TOKEN_H__
