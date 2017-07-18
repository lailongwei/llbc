/**
 * @file    BaseLogAppender.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_LOG_BASE_LOG_APPENDER_H__
#define __LLBC_CORE_LOG_BASE_LOG_APPENDER_H__

#include "llbc/common/Common.h"

#include "llbc/core/log/ILogAppender.h"

__LLBC_NS_BEGIN

/**
 * \brief The basic log appender class encapsulation.
 */
class LLBC_BaseLogAppender : public LLBC_ILogAppender
{
public:
    LLBC_BaseLogAppender();
    virtual ~LLBC_BaseLogAppender();

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

protected:
    /**
     * Get log level.
     * @return int - the log level.
     */
    virtual int GetLogLevel() const;

protected:

    /**
     * Get current appender's token chain.
     * @return LLBC_LogTokenChain * - the log token chain.
     */
    virtual LLBC_LogTokenChain *GetTokenChain() const;

protected:
    /**
     * Get next appender.
     * @return LLBC_ILogAppender * - next log appender.
     */
    virtual LLBC_ILogAppender *GetAppenderNext() const;

    /**
     * Set next appender.
     * @param[in] appender - next log appender.
     */
    virtual void SetAppenderNext(LLBC_ILogAppender *appender);

protected:
    /**
     * Flush method.
     */
    virtual void Flush();

private:
    int _level;

    LLBC_LogTokenChain *_chain;
    LLBC_ILogAppender *_next;
};

__LLBC_NS_END

#include "llbc/core/log/BaseLogAppenderImpl.h"

#endif // !__LLBC_CORE_LOG_BASE_LOG_APPENDER_H__
