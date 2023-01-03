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


#include "llbc/common/Export.h"

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/log/LogLevel.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_CString __level2StrRepr[LLBC_NS LLBC_LogLevel::End + 1] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",

    "UNKNOWN"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_CString &LLBC_LogLevel::GetLevelStr(int level)
{
    return ((level >= LLBC_LogLevel::Begin && level < LLBC_LogLevel::End) ?
        LLBC_INTERNAL_NS __level2StrRepr[level] : LLBC_INTERNAL_NS __level2StrRepr[LLBC_LogLevel::End]);
}

int LLBC_LogLevel::Str2Level(const LLBC_CString &levelStr)
{
    if (UNLIKELY(levelStr.empty()))
        return LLBC_LogLevel::End;

    const LLBC_String upperStr = LLBC_ToUpper(levelStr.c_str());
    if (LLBC_INTERNAL_NS __level2StrRepr[LLBC_LogLevel::Trace] == upperStr)
        return LLBC_LogLevel::Trace;
    else if (LLBC_INTERNAL_NS __level2StrRepr[LLBC_LogLevel::Debug] == upperStr)
        return LLBC_LogLevel::Debug;
    else if (LLBC_INTERNAL_NS __level2StrRepr[LLBC_LogLevel::Info] == upperStr)
        return LLBC_LogLevel::Info;
    else if (LLBC_INTERNAL_NS __level2StrRepr[LLBC_LogLevel::Warn] == upperStr)
        return LLBC_LogLevel::Warn;
    else if (LLBC_INTERNAL_NS __level2StrRepr[LLBC_LogLevel::Error] == upperStr)
        return LLBC_LogLevel::Error;
    else if (LLBC_INTERNAL_NS __level2StrRepr[LLBC_LogLevel::Fatal] == upperStr)
        return LLBC_LogLevel::Fatal;

    return LLBC_LogLevel::End;
}

bool LLBC_LogLevel::IsLegal(int level)
{
    return (LLBC_LogLevel::Begin <= level && level < LLBC_LogLevel::End);
}

__LLBC_NS_END
