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

#include "llbc/core/log/LogNetworkAppender.h"

__LLBC_NS_BEGIN

LLBC_LogNetworkAppender::LLBC_LogNetworkAppender()
: _ip("127.0.0.1")
, _port(0)
{
}

LLBC_LogNetworkAppender::~LLBC_LogNetworkAppender()
{
    Finalize();
}

int LLBC_LogNetworkAppender::GetType() const
{
    return LLBC_LogAppenderType::Network;
}

int LLBC_LogNetworkAppender::Initialize(const LLBC_LogAppenderInitInfo &initInfo)
{
    _ip = "127.0.0.1";
    _port = 0;

    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

void LLBC_LogNetworkAppender::Finalize()
{
    _Base::Finalize();
}

int LLBC_LogNetworkAppender::Output(const LLBC_LogData &data)
{
    LLBC_SetLastError(LLBC_ERROR_NOT_IMPL);
    return LLBC_FAILED;
}

__LLBC_NS_END
