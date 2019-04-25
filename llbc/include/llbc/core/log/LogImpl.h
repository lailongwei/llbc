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

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogJsonMsg.h"

#ifdef __LLBC_CORE_LOG_LOG_H__

__LLBC_NS_BEGIN

template <typename Tag>
inline void LLBC_LogHelper::d2(const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    d2(LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
inline void LLBC_LogHelper::d4(const char *logger, const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    d4(logger, LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jd2()
{
    return jd2(LLBC_GetTypeName(Tag));
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jd4(const char *logger)
{
    return jd4(logger, LLBC_GetTypeName(Tag));
}

template <typename Tag>
inline void LLBC_LogHelper::i2(const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    i2(LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
inline void LLBC_LogHelper::i4(const char *logger, const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    i4(logger, LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::ji2()
{
    return ji2(LLBC_GetTypeName(Tag));
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::ji4(const char *logger)
{
    return ji4(logger, LLBC_GetTypeName(Tag));
}

template <typename Tag>
inline void LLBC_LogHelper::w2(const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    w2(LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
inline void LLBC_LogHelper::w4(const char *logger, const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    w4(logger, LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jw2()
{
    return jw2(LLBC_GetTypeName(Tag));
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jw4(const char *logger)
{
    return jw4(logger, LLBC_GetTypeName(Tag));
}

template <typename Tag>
inline void LLBC_LogHelper::e2(const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    e2(LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
inline void LLBC_LogHelper::e4(const char *logger, const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    e4(logger, LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::je2()
{
    return je2(LLBC_GetTypeName(Tag));
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::je4(const char *logger)
{
    return je4(logger, LLBC_GetTypeName(Tag));
}

template <typename Tag>
inline void LLBC_LogHelper::f2(const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    f2(LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
inline void LLBC_LogHelper::f4(const char *logger, const char *fmt, ...)
{
    char *fmttedMsg; int msgLen;
    LLBC_FormatArg(fmt, fmttedMsg, msgLen);

    f4(logger, LLBC_GetTypeName(Tag), "%s", fmttedMsg);

    LLBC_Free(fmttedMsg);
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jf2()
{
    return jf2(LLBC_GetTypeName(Tag));
}

template <typename Tag>
LLBC_LogJsonMsg &LLBC_LogHelper::jf4(const char *logger)
{
    return jf4(logger, LLBC_GetTypeName(Tag));
}

__LLBC_NS_END

#endif // __LLBC_CORE_LOG_LOG_H__
