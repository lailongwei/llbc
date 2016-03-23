/**
 * @file    LogStrToken.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_STR_TOKEN_H__
#define __LLBC_CORE_LOG_LOG_STR_TOKEN_H__

#include "llbc/common/Common.h"

#include "llbc/core/log/BaseLogToken.h"

__LLBC_NS_BEGIN

/**
 * \brief String type log token calss encapsulation.
 */
class LLBC_HIDDEN LLBC_LogStrToken : public LLBC_BaseLogToken
{
public:
    LLBC_LogStrToken();
    virtual ~LLBC_LogStrToken();

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
    LLBC_String _str;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_STR_TOKEN_H__
