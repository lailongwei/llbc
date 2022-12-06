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

#include "llbc/core/log/LogLevel.h"
#include "llbc/core/log/LogTokenChain.h"
#include "llbc/core/log/BaseLogAppender.h"

__LLBC_NS_BEGIN

LLBC_BaseLogAppender::LLBC_BaseLogAppender()
: _level(LLBC_LogLevel::End)

, _chain(nullptr)
, _next(nullptr)
{
}

LLBC_BaseLogAppender::~LLBC_BaseLogAppender()
{
}

int LLBC_BaseLogAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    if (_chain)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_FAILED;
    }

    _level = initInfo.level;

    _chain = new LLBC_LogTokenChain;
    if (_chain->Build(initInfo.pattern) != LLBC_OK)
    {
        LLBC_XDelete(_chain);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

void LLBC_BaseLogAppender::Finalize()
{
    LLBC_XDelete(_chain);
    _level = LLBC_LogLevel::End;
}

__LLBC_NS_END
