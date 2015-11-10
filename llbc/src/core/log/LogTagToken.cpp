/**
 * @file    LogTagToken.cpp
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
#include "llbc/core/log/LogTagToken.h"

__LLBC_NS_BEGIN

LLBC_LogTagToken::LLBC_LogTagToken()
{
}

LLBC_LogTagToken::~LLBC_LogTagToken()
{
}

int LLBC_LogTagToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    this->SetFormatter(formatter);
    return LLBC_RTN_OK;
}

int LLBC_LogTagToken::GetType() const
{
    return LLBC_LogTokenType::TagToken;
}

void LLBC_LogTagToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());
    if (data.tagLen)
        formattedData.append(data.others + data.tagBeg, data.tagLen);

    LLBC_LogFormattingInfo *formatter = this->GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
