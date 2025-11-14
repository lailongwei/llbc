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

#include "llbc/core/log/LogTraceToken.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogLevel.h"

__LLBC_NS_BEGIN

LLBC_LogTraceToken::LLBC_LogTraceToken()
: _enableLogLevel(LLBC_LogLevel::Trace)
{
}

int LLBC_LogTraceToken::Initialize(const LLBC_LogFormattingInfo &formatter,
                                   const LLBC_LogTimeAccessor &logTimeAccessor,
                                   const LLBC_String &str)
{
    SetFormatter(formatter);

    const LLBC_String &rawParam = formatter.addiParam;
    const size_t colonPos = rawParam.find(':');
    if (colonPos != LLBC_String::npos)
    {
        if (rawParam.substr(0, colonPos).strip().tolower() == "enablelevel")
        {
            const LLBC_String valueStr = rawParam.substr(colonPos + 1).strip();
            _enableLogLevel = LLBC_LogLevel::GetLevelEnum(valueStr);
        }
    }

    return LLBC_OK;
}

int LLBC_LogTraceToken::GetType() const
{
    return LLBC_LogTokenType::LogTraceToken;
}

void LLBC_LogTraceToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    if (data.level < _enableLogLevel)
        return;

    auto logTrace = data.logTrace.get();
    if (logTrace && !logTrace->empty())
    {
        const int index = static_cast<int>(formattedData.size());

        formattedData.append(*logTrace);
        GetFormatter().Format(formattedData, index);
    }
}

__LLBC_NS_END
