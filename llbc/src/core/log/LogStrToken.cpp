/**
 * @file    LogStrToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogStrToken.h"

__LLBC_NS_BEGIN

LLBC_LogStrToken::LLBC_LogStrToken()
: _str()
{
}

LLBC_LogStrToken::~LLBC_LogStrToken()
{
}

int LLBC_LogStrToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    LLBC_XDelete(formatter);
    _str = str;

    return LLBC_OK;
}

int LLBC_LogStrToken::GetType() const
{
    return LLBC_LogTokenType::StrToken;
}

void LLBC_LogStrToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    formattedData.append(_str);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
