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

#include "llbc/core/file/Directory.h"

#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogExecNameToken.h"

__LLBC_NS_BEGIN

LLBC_LogExecNameToken::LLBC_LogExecNameToken()
: _execName{}
, _execNameLen(0)
{
}

LLBC_LogExecNameToken::~LLBC_LogExecNameToken()
{
}

int LLBC_LogExecNameToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    SetFormatter(formatter);

    const LLBC_String execName =
        LLBC_Directory::SplitExt(LLBC_Directory::ModuleFileName())[0];
    _execNameLen = MIN(execName.size(), sizeof(_execName));
    memcpy(_execName, execName.c_str(), _execNameLen);

    return LLBC_OK;
}

int LLBC_LogExecNameToken::GetType() const
{
    return LLBC_LogTokenType::ExecNameToken;
}

void LLBC_LogExecNameToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    const int index = static_cast<int>(formattedData.size());
    formattedData.append(_execName, _execNameLen);

    GetFormatter()->Format(formattedData, index);
}

__LLBC_NS_END
