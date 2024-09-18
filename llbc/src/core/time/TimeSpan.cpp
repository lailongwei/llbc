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

#include "llbc/core/time/TimeSpan.h"
#include "llbc/core/time/Time.h"

__LLBC_NS_BEGIN

const LLBC_TimeSpan LLBC_TimeSpan::zero = LLBC_TimeSpan::FromSeconds(0);
const LLBC_TimeSpan LLBC_TimeSpan::oneSec = LLBC_TimeSpan::FromSeconds(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneMillisec = LLBC_TimeSpan::FromMillis(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneMicrosec = LLBC_TimeSpan::FromMicros(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneMin = LLBC_TimeSpan::FromMinutes(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneHour = LLBC_TimeSpan::FromHours(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneDay = LLBC_TimeSpan::FromDays(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneWeek = LLBC_TimeSpan::FromDays(7);
const LLBC_TimeSpan LLBC_TimeSpan::negOneSec = LLBC_TimeSpan::FromSeconds(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneMillisec = LLBC_TimeSpan::FromMillis(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneMicrosec = LLBC_TimeSpan::FromMicros(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneMin = LLBC_TimeSpan::FromMinutes(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneHour = LLBC_TimeSpan::FromHours(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneDay = LLBC_TimeSpan::FromDays(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneWeek = LLBC_TimeSpan::FromDays(-7);

#define __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(partStr, partLen, spanFactor) \
    if (UNLIKELY(partLen >= sizeof(numFmtBuf)))                             \
    {                                                                       \
        _span = 0;                                                          \
        return;                                                             \
    }                                                                       \
                                                                            \
    if (LIKELY(partLen > 0))                                                \
    {                                                                       \
        memcpy(numFmtBuf, partStr, partLen);                                \
        numFmtBuf[partLen] = '\0';                                          \
        _span += atoi(numFmtBuf) * spanFactor;                              \
    }                                                                       \

LLBC_TimeSpan::LLBC_TimeSpan(const char *spanStr, size_t spanStrLen)
: _span(0)
{
    // Time span string format:
    // - [DD ][[HH:]MM:]SS[.micro_sec]
    //
    // eg:
    //                   DD       HH       MM   SS   micro_sec
    // 03 =>                                     3 +     0      seconds
    // 02:03 =>                           2*60 + 3 +     0      seconds
    // 01:02:03 =>               1*3600 + 2*60 + 3 +     0      seconds
    // 8 01:02:03 =>   8*86400 + 1*3600 + 2*60 + 3 +     0      seconds
    // 8 01:02:03.4 => 8*86400 + 1*3600 + 2*60 + 3 + 4*0.000001 seconds

    // Argument check.
    if (UNLIKELY(!spanStr || spanStrLen == 0))
        return;

    // Remove leading whitespace characters.
    const char *origSpanStr = spanStr;
    while (*spanStr != '\0')
    {
        if (LIKELY(!LLBC_IsSpace(*spanStr)))
            break;
        ++spanStr;
    }

    if (*spanStr == '\0')
        return;
    spanStrLen -= static_cast<size_t>(spanStr - origSpanStr);

    // Remove trailing whitespace characters.
    const char *spanStrEnd = spanStr + spanStrLen - 1;
    while (spanStrEnd != spanStr && LLBC_IsSpace(*spanStrEnd))
        --spanStrEnd;
    ++spanStrEnd;

    // Parse time span string.
    // - Parse <DD> part.
    char numFmtBuf[12];
    const char *dayPartEnd = spanStr;
    while (++dayPartEnd != spanStrEnd)
        LLBC_BreakIf(LLBC_IsSpace(*dayPartEnd));
    if (dayPartEnd != spanStrEnd)
    {
        size_t dayPartLen = static_cast<size_t>(dayPartEnd - spanStr);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(spanStr, dayPartLen, LLBC_TimeConst::numOfMicrosPerDay);

        spanStr = dayPartEnd + 1;
    }

    // - Search all colons pos.
    size_t colonSize = 0;
    const char *colonPoses[2]{nullptr, nullptr};
    const char *strIt = spanStr;
    while (strIt != spanStrEnd)
    {
        if (*strIt == ':')
        {
            colonPoses[colonSize++] = strIt;
            if (colonSize == 2)
                break;
        }

        ++strIt;
    }

    // - Parse <HH:MM> part.
    const char *secPart;
    if (colonSize == 2)
    {
        size_t partLen = static_cast<size_t>(colonPoses[0] - spanStr);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(spanStr, partLen, LLBC_TimeConst::numOfMicrosPerHour);

        partLen = static_cast<size_t>(colonPoses[1] - colonPoses[0] - 1);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(colonPoses[0] + 1, partLen, LLBC_TimeConst::numOfMicrosPerMinute);

        secPart = colonPoses[1] + 1;
    }
    else if (colonSize == 1)
    {
        size_t partLen = static_cast<size_t>(colonPoses[0] - spanStr);
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(spanStr, partLen, LLBC_TimeConst::numOfMicrosPerMinute);

        secPart = colonPoses[0] + 1;
    }
    else
    {
        secPart = spanStr;
    }

    // - Parse <SS.micro_sec> part.
    const char *dotPos = secPart;
    while (dotPos != spanStrEnd)
    {
        if (*dotPos == '.')
            break;

        ++dotPos;
    }

    const char *secPartEnd = dotPos != spanStrEnd ? dotPos : spanStrEnd;
    size_t secPartLen = static_cast<size_t>(secPartEnd - secPart);
    __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(secPart, secPartLen, LLBC_TimeConst::numOfMicrosPerSecond);
    if (dotPos != spanStrEnd)
    {
        size_t microPartLen = spanStrEnd - dotPos - 1;
        __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN(dotPos + 1, microPartLen, 1);
    }
}

#undef __LLBC_INL_TIME_SPAN_STR_PART_TO_SPAN

__LLBC_NS_END
