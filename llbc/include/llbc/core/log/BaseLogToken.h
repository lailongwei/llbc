/**
 * @file    BaseLogToken.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_BASE_LOG_TOKEN_H__
#define __LLBC_CORE_LOG_BASE_LOG_TOKEN_H__

#include "llbc/common/Common.h"

#include "llbc/core/log/ILogToken.h"

__LLBC_NS_BEGIN

/**
 * \brief The basic log token class encapsulation.
 */
class LLBC_HIDDEN LLBC_BaseLogToken : public LLBC_ILogToken
{
public:
    LLBC_BaseLogToken();
    virtual ~LLBC_BaseLogToken();

protected:
    /**
     * Get token log formatter.
     * @return LLBC_LogFormattingInfo * - log formatter.
     */
    virtual LLBC_LogFormattingInfo *GetFormatter() const;

    /**
     * Set token log formatter.
     * @param[in] formatter - log formatter.
     */
    virtual void SetFormatter(LLBC_LogFormattingInfo *formatter);

protected:
    friend class LLBC_LogTokenChain;

    /**
     * Get next token.
     * @return LLBC_ILogToken * - next log token.
     */
    virtual LLBC_ILogToken *GetTokenNext() const;

    /**
     * Set next token.
     * @param[in] next - next log token.
     */
    virtual void SetTokenNext(LLBC_ILogToken *next);

private:
    LLBC_LogFormattingInfo *_formatter;
    LLBC_ILogToken *_nextToken;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_BASE_LOG_TOKEN_H__
