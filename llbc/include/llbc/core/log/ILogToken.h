/**
 * @file    ILogToken.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_ILOG_TOKEN_H__
#define __LLBC_CORE_LOG_ILOG_TOKEN_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
struct LLBC_LogData;
class LLBC_LogFormattingInfo;

class LLBC_LogTokenChain;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The log token type encapsulation.
 */
class LLBC_LogTokenType
{
public:
    /**
     * Token type enumeration.
     */
    enum
    {
        StrToken      = '\0',         // ..: string type token.
        NameToken     = 'N',          // %L: logger name type token.
        TagToken      = 'g',          // %g: tag type token.
        FileToken     = 'f',          // %f: file name type token.
        LineToken     = 'l',          // %l: file line type token.
        ThreadIdToken = 't',          // %t: thread id type token.
        LevelToken    = 'L',          // %L: level type token, like DEBUG, INFO, eg ...
        NewLineToken  = 'n',          // %n: new line type.
        MsgToken      = 'm',          // %m: message type token.
        TimeToken     = 'T',          // %T: time token.
        EscapeToken   = '%',          // %%: escape token.

        End
    };
};

/**
 * \brief The log token interface class encapsulation.
 */
class LLBC_ILogToken
{
public:
    virtual ~LLBC_ILogToken() {  }

public:
    /**
     * Initialize the log token.
     * @param[in] formatter - log formatter.
     * @param[in] str       - token append string data.
     * @return int - return 0 if success, otherwise return -1.
     */
    virtual int Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str) = 0;

    /**
     * Get token type.
     * @return int - token type.
     */
    virtual int GetType() const = 0;

    /**
     * Format the log data.
     * @param[in] data           - log data.
     * @param[out] formattedData - store location for formatted log string.
     */
    virtual void Format(const LLBC_LogData &data, LLBC_String &formattedData) const = 0;

protected:
    /**
     * Get token log formatter.
     * @return LLBC_LogFormattingInfo * - log formatter.
     */
    virtual LLBC_LogFormattingInfo *GetFormatter() const = 0;

    /**
     * Set token log formatter.
     * @param[in] formatter - log formatter.
     */
    virtual void SetFormatter(LLBC_LogFormattingInfo *formatter) = 0;

protected:
    friend class LLBC_LogTokenChain;

    /**
     * Get next token.
     * @return LLBC_ILogToken * - next log token.
     */
    virtual LLBC_ILogToken *GetTokenNext() const = 0;

    /**
     * Set next token.
     * @param[in] next - next log token.
     */
    virtual void SetTokenNext(LLBC_ILogToken *next) = 0;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_LOG_ILOG_TOKEN_H__
