/**
 * @file    LogTokenBuilder.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

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
