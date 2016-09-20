/**
 * @file    LogTokenChain.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_LOG_TOKEN_CHAIN_H__
#define __LLBC_CORE_LOG_LOG_TOKEN_CHAIN_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_ILogToken;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log token chain class encapsulation.
 */
class LLBC_EXPORT LLBC_LogTokenChain
{
public:
    LLBC_LogTokenChain();
    ~LLBC_LogTokenChain();

public:
    /**
     * Build token chain by given pattern string.
     * @param[in] pattern - the log pattern.
     * @return int - return 0 if successed, otherwise return -1.
     */
    int Build(const LLBC_String &pattern);

    /**
     * Format specific log data.
     * @param[in] data - log data.
     * @param[out] storage location for formatted log message.
     */
    void Format(const LLBC_LogData &data, LLBC_String &formattedData) const;

    /**
     * Clean the log token chain.
     */
    void Cleanup();

private:
    /**
     * Append log token.
     * @param[in] token - the log token.
     */
    void AppendToken(LLBC_ILogToken *token);

private:
    LLBC_ILogToken *_head;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_LOG_TOKEN_CHAIN_H__
