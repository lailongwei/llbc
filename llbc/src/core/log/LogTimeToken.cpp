/**
 * @file    LogTimeToken.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/10
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/time/Time.h"

#include "llbc/core/log/LogData.h"
#include "llbc/core/log/LogFormattingInfo.h"
#include "llbc/core/log/LogTimeToken.h"

__LLBC_NS_BEGIN

LLBC_LogTimeToken::LLBC_LogTimeToken()
{
}

LLBC_LogTimeToken::~LLBC_LogTimeToken()
{
}

int LLBC_LogTimeToken::Initialize(LLBC_LogFormattingInfo *formatter, const LLBC_String &str)
{
    this->SetFormatter(formatter);
    return LLBC_RTN_OK;
}

int LLBC_LogTimeToken::GetType() const
{
    return LLBC_LogTokenType::TimeToken;
}

void LLBC_LogTimeToken::Format(const LLBC_LogData &data, LLBC_String &formattedData) const
{
    // Format non millisecond part.
    int index = static_cast<int>(formattedData.size());
    time_t timeInSecond = static_cast<time_t>(data.logTime / 1000);

    struct tm timeStruct;
#if LLBC_TARGET_PLATFORM_WIN32
    localtime_s(&timeStruct, &timeInSecond);
#else
    localtime_r(&timeInSecond, &timeStruct);
#endif

    char fmttedBuf[19];
    strftime(fmttedBuf, sizeof(fmttedBuf), "%y-%m-%d %H:%M:%S.", &timeStruct);

    formattedData.append(fmttedBuf, 18);

    // Format millisecond part.
    formattedData.append_format("%03llu", data.logTime % 1000);

    LLBC_LogFormattingInfo *formatter = this->GetFormatter();
    formatter->Format(formattedData, index);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
