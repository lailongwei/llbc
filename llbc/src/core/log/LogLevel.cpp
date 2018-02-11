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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/log/LogLevel.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_String __level2StrDesc[LLBC_NS LLBC_LogLevel::End + 1] =
{
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",

    "UNKNOWN"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_String &LLBC_LogLevel::GetDebugDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Debug];
}

const LLBC_String &LLBC_LogLevel::GetInfoDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Info];
}

const LLBC_String &LLBC_LogLevel::GetWarnDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Warn];
}

const LLBC_String &LLBC_LogLevel::GetErrorDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Error];
}

const LLBC_String &LLBC_LogLevel::GetFatalDesc()
{
    return LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Fatal];
}

const LLBC_String &LLBC_LogLevel::GetLevelDesc(int level)
{
    return ((level >= LLBC_LogLevel::Begin && level < LLBC_LogLevel::End) ?
        LLBC_INTERNAL_NS __level2StrDesc[level] : LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::End]);
}

int LLBC_LogLevel::Str2Level(const char *level)
{
    if (UNLIKELY(!level))
    {
        return LLBC_LogLevel::End;
    }

    LLBC_String upperStr = LLBC_ToUpper(level);
    if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Debug])
    {
        return LLBC_LogLevel::Debug;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Info])
    {
        return LLBC_LogLevel::Info;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Warn])
    {
        return LLBC_LogLevel::Warn;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Error])
    {
        return LLBC_LogLevel::Error;
    }
    else if (upperStr == LLBC_INTERNAL_NS __level2StrDesc[LLBC_LogLevel::Fatal])
    {
        return LLBC_LogLevel::Fatal;
    }

    return LLBC_LogLevel::End;
}

bool LLBC_LogLevel::IsLegal(int level)
{
    return (LLBC_LogLevel::Begin <= level && level < LLBC_LogLevel::End);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
