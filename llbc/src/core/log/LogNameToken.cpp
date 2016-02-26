/**
 * @file    LogNameToken.cpp
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
#include "llbc/core/log/LogNameToken.h"

__LLBC_NS_BEGIN

LLBC_LogNameToken::LLBC_LogNameToken()
{
}

LLBC_LogNameToken::~LLBC_LogNameToken()
{
}

int LLBC_LogNameToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogNameToken::GetType() const
{
    return LLBC_LogTokenType::NameToken;
}

void LLBC_LogNameToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());
    formattedData.append(data.loggerName);

    LLBC_LogFormattingInfo *formatter = GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
