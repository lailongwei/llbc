/**
 * @file    LogMessageBufferImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/11/16
 * @version 1.0
 *
 * @brief
 */

#ifdef __LLBC_CORE_LOG_LOG_MESSAGE_BUFFER_H__

template <typename T>
std::basic_ostream<char> &operator <<(LLBC_NS LLBC_LogMessageBuffer &msg, const T &val)
{
    return static_cast<std::basic_ostream<char> &>(msg) << val;
}

#endif // __LLBC_CORE_LOG_LOG_MESSAGE_BUFFER_H__
