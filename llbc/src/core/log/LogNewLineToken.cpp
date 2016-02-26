/**
 * @file    LogNewLineToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

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

#include "llbc/common/AfterIncl.h"
