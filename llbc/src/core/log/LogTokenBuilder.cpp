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

#include "llbc/core/log/LogNullToken.h"
#include "llbc/core/log/LogStrToken.h"
#include "llbc/core/log/LogNameToken.h"
#include "llbc/core/log/LogExecNameToken.h"
#include "llbc/core/log/LogTagToken.h"
#include "llbc/core/log/LogFileToken.h"
#include "llbc/core/log/LogLineToken.h"
#include "llbc/core/log/LogFunctionToken.h"
#include "llbc/core/log/LogProcessIdToken.h"
#include "llbc/core/log/LogThreadIdToken.h"
#include "llbc/core/log/LogLevelToken.h"
#include "llbc/core/log/LogNewLineToken.h"
#include "llbc/core/log/LogMsgToken.h"
#include "llbc/core/log/LogTimeToken.h"
#include "llbc/core/log/LogEscapeToken.h"
#include "llbc/core/log/LogTokenBuilder.h"

__LLBC_NS_BEGIN

LLBC_LogTokenBuilder::LLBC_LogTokenBuilder()
{
}

LLBC_LogTokenBuilder::~LLBC_LogTokenBuilder()
{
}

LLBC_ILogToken *LLBC_LogTokenBuilder::BuildLogToken(int type) const
{
    switch(type)
    {
    case LLBC_LogTokenType::StrToken:
        return new LLBC_LogStrToken;

    case LLBC_LogTokenType::NameToken:
        return new LLBC_LogNameToken;

    case LLBC_LogTokenType::ExecNameToken:
        return new LLBC_LogExecNameToken;

    case LLBC_LogTokenType::TagToken:
        return new LLBC_LogTagToken;

    case LLBC_LogTokenType::FileToken:
        return new LLBC_LogFileToken;

    case LLBC_LogTokenType::LineToken:
        return new LLBC_LogLineToken;

    case LLBC_LogTokenType::FunctionToken:
        return new LLBC_LogFunctionToken;

    case LLBC_LogTokenType::ThreadIdToken:
        return new LLBC_LogThreadIdToken;
        break;

    case LLBC_LogTokenType::ProcessIdToken:
        return new LLBC_LogProcessIdToken;

    case LLBC_LogTokenType::LevelToken:
        return new LLBC_LogLevelToken;

    case LLBC_LogTokenType::NewLineToken:
        return new LLBC_LogNewLineToken;

    case LLBC_LogTokenType::MsgToken:
        return new LLBC_LogMsgToken;

    case LLBC_LogTokenType::TimeToken:
        return new LLBC_LogTimeToken;

    case LLBC_LogTokenType::EscapeToken:
        return new LLBC_LogEscapeToken;

    default:
        return new LLBC_LogNullToken;
    }
}

__LLBC_NS_END
