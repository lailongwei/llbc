/**
 * @file    LogEscapeToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogEscapeToken.h"

__LLBC_NS_BEGIN

LLBC_LogEscapeToken::LLBC_LogEscapeToken()
: LLBC_BaseLogToken()
{
}

LLBC_LogEscapeToken::~LLBC_LogEscapeToken()
{
}

int LLBC_LogEscapeToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    this->SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogEscapeToken::GetType() const
{
    return LLBC_LogTokenType::EscapeToken;
}

void LLBC_LogEscapeToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());
    formattedData.append(1, static_cast<char>(LLBC_LogTokenType::EscapeToken));

    LLBC_LogFormattingInfo *formatter = this->GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
