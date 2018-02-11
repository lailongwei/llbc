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

#include "llbc/core/log/LogStrToken.h"
#include "llbc/core/log/LogNullToken.h"
#include "llbc/core/log/LogNameToken.h"
#include "llbc/core/log/LogTagToken.h"
#include "llbc/core/log/LogFileToken.h"
#include "llbc/core/log/LogLineToken.h"
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
    LLBC_ILogToken *token = NULL;

    switch(type)
    {
    case LLBC_LogTokenType::StrToken:
        token = new LLBC_LogStrToken;
        break;

    case LLBC_LogTokenType::NameToken:
        token = new LLBC_LogNameToken;
        break;

    case LLBC_LogTokenType::TagToken:
        token = new LLBC_LogTagToken;
        break;

    case LLBC_LogTokenType::FileToken:
        token = new LLBC_LogFileToken;
        break;

    case LLBC_LogTokenType::LineToken:
        token = new LLBC_LogLineToken;
        break;

    case LLBC_LogTokenType::ThreadIdToken:
        token = new LLBC_LogThreadIdToken;
        break;

    case LLBC_LogTokenType::ProcessIdToken:
        token = new LLBC_LogProcessIdToken;
        break;

    case LLBC_LogTokenType::LevelToken:
        token = new LLBC_LogLevelToken;
        break;

    case LLBC_LogTokenType::NewLineToken:
        token = new LLBC_LogNewLineToken;
        break;

    case LLBC_LogTokenType::MsgToken:
        token = new LLBC_LogMsgToken;
        break;

    case LLBC_LogTokenType::TimeToken:
        token = new LLBC_LogTimeToken;
        break;

    case LLBC_LogTokenType::EscapeToken:
        token = new LLBC_LogEscapeToken;
        break;

    default:
        token = new LLBC_LogNullToken;
        break;
    }

    return token;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
