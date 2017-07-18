/**
 * @file    BaseLogAppenderImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017/07/18
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_LOG_BASE_LOG_APPENDER_H__

__LLBC_NS_BEGIN

inline int LLBC_BaseLogAppender::GetLogLevel() const
{
    return _level;
}

inline LLBC_LogTokenChain *LLBC_BaseLogAppender::GetTokenChain() const
{
    return _chain;
}

inline LLBC_ILogAppender *LLBC_BaseLogAppender::GetAppenderNext() const
{
    return _next;
}

inline void LLBC_BaseLogAppender::SetAppenderNext(LLBC_ILogAppender *next)
{
    _next = next;
}

__LLBC_NS_END

#endif // __LLBC_CORE_LOG_BASE_LOG_APPENDER_H__