/**
 * @file    LogNullToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogNullToken.h"

__LLBC_NS_BEGIN

LLBC_LogNullToken::LLBC_LogNullToken()
{
}

LLBC_LogNullToken::~LLBC_LogNullToken()
{
}

int LLBC_LogNullToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    LLBC_XDelete(formatter);

    return LLBC_OK;
}

int LLBC_LogNullToken::GetType() const
{
    return LLBC_LogTokenType::End;
}

void LLBC_LogNullToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
