/**
 * @file    LogLineToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogLineToken.h"

__LLBC_NS_BEGIN

LLBC_LogLineToken::LLBC_LogLineToken()
{
}

LLBC_LogLineToken::~LLBC_LogLineToken()
{
}

int LLBC_LogLineToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    this->SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogLineToken::GetType() const
{
    return LLBC_LogTokenType::LineToken;
}

void LLBC_LogLineToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());
    formattedData.append(LLBC_Num2Str(data.line));

    LLBC_LogFormattingInfo *formatter = this->GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
