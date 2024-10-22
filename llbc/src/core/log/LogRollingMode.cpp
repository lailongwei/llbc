// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "llbc/common/Export.h"

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/log/LogRollingMode.h"

__LLBC_INTERNAL_NS_BEGIN

static const LLBC_NS LLBC_CString __rollMode2StrRepr[LLBC_NS LLBC_LogRollingMode::End + 1] =
{
    "NOROLLING", // NoRolling
    "HOURLYROLLING", // Hourly Rolling
    "DAILYROLLING", // Daily Rolling

    "UKNLOGROLLING" // Unknown Log Rolling
};

static const LLBC_NS LLBC_CString __rollMode2StrAliasRepr[LLBC_NS LLBC_LogRollingMode::End + 1] =
{
    "NO", // NoRolling
    "HOURLY", // Hourly Rolling
    "DAILY", // Daily Rolling

    "UKNLOGROLLING" // Unknown Log Rolling
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

const LLBC_CString &LLBC_LogRollingMode::GetModeStr(int mode)
{
    return IsValid(mode) ?
        LLBC_INTERNAL_NS __rollMode2StrRepr[mode] :
            LLBC_INTERNAL_NS __rollMode2StrRepr[End];
}

int LLBC_LogRollingMode::Str2Mode(const LLBC_CString &modeStr)
{
    const LLBC_String upperModeStr = LLBC_ToUpper(modeStr.c_str());
    if (LLBC_INTERNAL_NS __rollMode2StrRepr[HourlyRolling] == upperModeStr ||
        LLBC_INTERNAL_NS __rollMode2StrAliasRepr[HourlyRolling] == upperModeStr)
        return HourlyRolling;
    else if (LLBC_INTERNAL_NS __rollMode2StrRepr[DailyRolling] == upperModeStr ||
        LLBC_INTERNAL_NS __rollMode2StrAliasRepr[DailyRolling] == upperModeStr)
        return DailyRolling;
    else if (LLBC_INTERNAL_NS __rollMode2StrRepr[NoRolling] == upperModeStr ||
        LLBC_INTERNAL_NS __rollMode2StrAliasRepr[NoRolling] == upperModeStr)
        return NoRolling;
    else
        return End;
}

bool LLBC_LogRollingMode::IsValid(int mode)
{
    return mode >= Begin && mode < End;
}

__LLBC_NS_END
