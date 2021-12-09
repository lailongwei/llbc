// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifdef __LLBC_CORE_LOG_LOG_H__

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogJsonMsg.h"

__LLBC_NS_BEGIN

#define __LLBC_DO_LOG(loggerName, level, tag, fmt)                    \
    do                                                                \
    {                                                                 \
        if (UNLIKELY(!_rootLogger))                                   \
        {                                                             \
            char *fmttedMsg; int msgLen;                              \
            LLBC_FormatArg(fmt, fmttedMsg, msgLen);                   \
            UnInitOutput(level >= LLBC_NS LLBC_LogLevel::Warn ? stderr : stdout, fmttedMsg); \
                                                                      \
            LLBC_Free(fmttedMsg);                                     \
            break;                                                    \
        }                                                             \
                                                                      \
        LLBC_NS LLBC_Logger *l = nullptr;                             \
        if (!loggerName)                                              \
            l = _rootLogger;                                          \
        else                                                          \
            l = _loggerManager->GetLogger(loggerName);                \
                                                                      \
        if (LIKELY(l))                                                \
        {                                                             \
            va_list va;                                               \
            va_start(va, fmt);                                        \
            l->FormatOutput(level, tag, __FILE__, __LINE__, fmt, va); \
            va_end(va);                                               \
        }                                                             \
    } while (0)                                                       \

#define __LLBC_DO_JLOG(loggerName, tag, lv)                           \
    LLBC_Logger *l = nullptr;                                         \
    if (!loggerName)                                                  \
        l = _rootLogger;                                              \
    else if (LIKELY(_loggerManager))                                  \
        l = _loggerManager->GetLogger(loggerName);                    \
                                                                      \
    return *LLBC_New(LLBC_LogJsonMsg, l, tag, lv);                    \


inline void LLBC_LogHelper::d(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Debug, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::d2(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Debug, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::d2(const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Debug, tag, fmt);
}

inline void LLBC_LogHelper::d3(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Debug, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::d4(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Debug, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::d4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Debug, tag, fmt);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jd()
{
    __LLBC_DO_JLOG(nullptr, nullptr, LLBC_LogLevel::Debug);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jd2()
{
    __LLBC_DO_JLOG(nullptr, LLBC_GetTypeName(Tag), LLBC_LogLevel::Debug);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jd2(const char *tag)
{
    __LLBC_DO_JLOG(nullptr, tag, LLBC_LogLevel::Debug);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jd3(const char *logger)
{
    __LLBC_DO_JLOG(logger, nullptr, LLBC_LogLevel::Debug);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jd4(const char *logger)
{
    __LLBC_DO_JLOG(logger, LLBC_GetTypeName(Tag), LLBC_LogLevel::Debug);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jd4(const char *logger, const char *tag)
{
    __LLBC_DO_JLOG(logger, tag, LLBC_LogLevel::Debug);
}

inline void LLBC_LogHelper::i(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Info, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::i2(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Info, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::i2(const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Info, tag, fmt);
}

inline void LLBC_LogHelper::i3(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Info, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::i4(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Info, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::i4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Info, tag, fmt);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::ji()
{
    __LLBC_DO_JLOG(nullptr, nullptr, LLBC_LogLevel::Info);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::ji2()
{
    __LLBC_DO_JLOG(nullptr, LLBC_GetTypeName(Tag), LLBC_LogLevel::Info);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::ji2(const char *tag)
{
    __LLBC_DO_JLOG(nullptr, tag, LLBC_LogLevel::Info);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::ji3(const char *logger)
{
    __LLBC_DO_JLOG(logger, nullptr, LLBC_LogLevel::Info);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::ji4(const char *logger)
{
    __LLBC_DO_JLOG(logger, LLBC_GetTypeName(Tag), LLBC_LogLevel::Info);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::ji4(const char *logger, const char *tag)
{
    __LLBC_DO_JLOG(logger, tag, LLBC_LogLevel::Info);
}

inline void LLBC_LogHelper::w(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Warn, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::w2(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Warn, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::w2(const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Warn, tag, fmt);
}

inline void LLBC_LogHelper::w3(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Warn, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::w4(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Warn, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::w4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Warn, tag, fmt);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jw()
{
    __LLBC_DO_JLOG(nullptr, nullptr, LLBC_LogLevel::Warn);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jw2()
{
    __LLBC_DO_JLOG(nullptr, LLBC_GetTypeName(Tag), LLBC_LogLevel::Warn);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jw2(const char *tag)
{
    __LLBC_DO_JLOG(nullptr, tag, LLBC_LogLevel::Warn);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jw3(const char *logger)
{
    __LLBC_DO_JLOG(logger, nullptr, LLBC_LogLevel::Warn);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jw4(const char *logger)
{
    __LLBC_DO_JLOG(logger, LLBC_GetTypeName(Tag), LLBC_LogLevel::Warn);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jw4(const char *logger, const char *tag)
{
    __LLBC_DO_JLOG(logger, tag, LLBC_LogLevel::Warn);
}

inline void LLBC_LogHelper::e(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Error, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::e2(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Error, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::e2(const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Error, tag, fmt);
}

inline void LLBC_LogHelper::e3(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Error, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::e4(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Error, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::e4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Error, tag, fmt);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::je()
{
    __LLBC_DO_JLOG(nullptr, nullptr, LLBC_LogLevel::Error);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::je2()
{
    __LLBC_DO_JLOG(nullptr, LLBC_GetTypeName(Tag), LLBC_LogLevel::Error);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::je2(const char *tag)
{
    __LLBC_DO_JLOG(nullptr, tag, LLBC_LogLevel::Error);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::je3(const char *logger)
{
    __LLBC_DO_JLOG(logger, nullptr, LLBC_LogLevel::Error);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::je4(const char *logger)
{
    __LLBC_DO_JLOG(logger, LLBC_GetTypeName(Tag), LLBC_LogLevel::Error);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::je4(const char *logger, const char *tag)
{
    __LLBC_DO_JLOG(logger, tag, LLBC_LogLevel::Error);
}

inline void LLBC_LogHelper::f(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Fatal, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::f2(const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Fatal, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::f2(const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(nullptr, LLBC_LogLevel::Fatal, tag, fmt);
}

inline void LLBC_LogHelper::f3(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Fatal, nullptr, fmt);
}

template <typename Tag>
void LLBC_LogHelper::f4(const char *logger, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Fatal, LLBC_GetTypeName(Tag), fmt);
}

inline void LLBC_LogHelper::f4(const char *logger, const char *tag, const char *fmt, ...)
{
    __LLBC_DO_LOG(logger, LLBC_LogLevel::Fatal, tag, fmt);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jf()
{
    __LLBC_DO_JLOG(nullptr, nullptr, LLBC_LogLevel::Fatal);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jf2()
{
    __LLBC_DO_JLOG(nullptr, LLBC_GetTypeName(Tag), LLBC_LogLevel::Fatal);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jf2(const char *tag)
{
    __LLBC_DO_JLOG(nullptr, tag, LLBC_LogLevel::Fatal);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jf3(const char *logger)
{
    __LLBC_DO_JLOG(logger, nullptr, LLBC_LogLevel::Fatal);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jf4(const char *logger)
{
    __LLBC_DO_JLOG(logger, LLBC_GetTypeName(Tag), LLBC_LogLevel::Fatal);
}

inline LLBC_LogJsonMsg &LLBC_LogHelper::jf4(const char *logger, const char *tag)
{
    __LLBC_DO_JLOG(logger, tag, LLBC_LogLevel::Fatal);
}

//! At latest, undef code define macros.
#undef __LLBC_DO_LOG
#undef __LLBC_DO_JLOG

__LLBC_NS_END

#endif // __LLBC_CORE_LOG_LOG_H__
