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

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogNewLineToken.h"

__LLBC_NS_BEGIN

LLBC_LogNewLineToken::LLBC_LogNewLineToken()
{
}

LLBC_LogNewLineToken::~LLBC_LogNewLineToken()
{
}

int LLBC_LogNewLineToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogNewLineToken::GetType() const
{
    return LLBC_LogTokenType::NewLineToken;
}

void LLBC_LogNewLineToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());
    formattedData.append(1, LLBC_LF_A);

    LLBC_LogFormattingInfo *formatter = GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END
