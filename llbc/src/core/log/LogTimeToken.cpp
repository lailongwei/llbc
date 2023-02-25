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

#include "llbc/core/time/Time.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogTimeToken.h"

__LLBC_NS_BEGIN

LLBC_LogTimeToken::LLBC_LogTimeToken()
: _lastFmtTime(0)
, _cacheLen(0)
, _fmtCache{}
{
}

LLBC_LogTimeToken::~LLBC_LogTimeToken()
{
}

int LLBC_LogTimeToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogTimeToken::GetType() const
{
    return LLBC_LogTokenType::TimeToken;
}

void LLBC_LogTimeToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    // Format non millisecond part.
    const int index = static_cast<int>(formattedData.size());
    const time_t timeInSecond = static_cast<time_t>(data.logTime / 1000000);

    if (timeInSecond != _lastFmtTime)
    {
        struct tm timeStruct;
    #if LLBC_TARGET_PLATFORM_WIN32
        localtime_s(&timeStruct, &timeInSecond);
    #else
        localtime_r(&timeInSecond, &timeStruct);
    #endif
        _lastFmtTime = timeInSecond;
        _cacheLen = strftime(_fmtCache,
                             sizeof(_fmtCache),
                             "%y-%m-%d %H:%M:%S.", &timeStruct);
    }

    formattedData.append(_fmtCache, _cacheLen);

    // Format millisecond part.
    formattedData.append_format("%06llu", data.logTime % 1000000);

    GetFormatter()->Format(formattedData, index);
}

__LLBC_NS_END
