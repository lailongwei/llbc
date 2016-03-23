/**
 * @file    LogProcessIdToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/03/23
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Process.h"
#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogProcessIdToken.h"

__LLBC_NS_BEGIN

LLBC_LogProcessIdToken::LLBC_LogProcessIdToken()
: _processId(LLBC_GetCurrentProcessId())
{
}

LLBC_LogProcessIdToken::~LLBC_LogProcessIdToken()
{
}

int LLBC_LogProcessIdToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    SetFormatter(formatter);
    return LLBC_OK;
}

int LLBC_LogProcessIdToken::GetType() const
{
    return LLBC_LogTokenType::ProcessIdToken;
}

void LLBC_LogProcessIdToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    int index = static_cast<int>(formattedData.size());
    formattedData.append(LLBC_Num2Str(_processId));

    LLBC_LogFormattingInfo *formatter = GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
