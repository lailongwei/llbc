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

#include "llbc/core/log/LogConsoleAppender.h"
#include "llbc/core/log/LogFileAppender.h"
#include "llbc/core/log/LogNetworkAppender.h"

#include "llbc/core/log/LogAppenderBuilder.h"

__LLBC_NS_BEGIN

LLBC_LogAppenderBuilder::LLBC_LogAppenderBuilder()
{
}

LLBC_LogAppenderBuilder::~LLBC_LogAppenderBuilder()
{
}

LLBC_ILogAppender *LLBC_LogAppenderBuilder::BuildAppender(int type) const
{
    LLBC_ILogAppender *appender = nullptr;
    switch (type)
    {
    case LLBC_LogAppenderType::Console:
        appender = new LLBC_LogConsoleAppender;
        break;

    case LLBC_LogAppenderType::File:
        appender = new LLBC_LogFileAppender;
        break;

    case LLBC_LogAppenderType::Network:
        appender = new LLBC_LogNetworkAppender;
        break;

    default:
        break;
    }

    return appender;
}

__LLBC_NS_END
