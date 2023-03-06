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

#include "llbc/core/os/OS_Process.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogProcessIdToken.h"

__LLBC_NS_BEGIN

LLBC_LogProcessIdToken::LLBC_LogProcessIdToken()
: _processId(LLBC_GetCurrentProcessId())
{
}

LLBC_LogProcessIdToken::~LLBC_LogProcessIdToken()
{
}

int LLBC_LogProcessIdToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogProcessIdToken::GetType() const
{
    return LLBC_LogTokenType::ProcessIdToken;
}

void LLBC_LogProcessIdToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", _processId);
    formattedData.append(buf);

    GetFormatter()->Format(formattedData, index);
}

__LLBC_NS_END
