/**
 * @file    LogFileToken.cpp
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
#include "llbc/core/log/LogFileToken.h"

__LLBC_NS_BEGIN

LLBC_LogFileToken::LLBC_LogFileToken()
{
}

LLBC_LogFileToken::~LLBC_LogFileToken()
{
}

int LLBC_LogFileToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    this->SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogFileToken::GetType() const
{
    return LLBC_LogTokenType::FileToken;
}

void LLBC_LogFileToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());
    if (data.fileLen)
        formattedData.append(data.others + data.fileBeg, data.fileLen);

    LLBC_LogFormattingInfo *formatter = this->GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
