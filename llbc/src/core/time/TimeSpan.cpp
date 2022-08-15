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

#include "llbc/core/time/TimeSpan.h"
#include "llbc/core/time/Time.h"

__LLBC_NS_BEGIN


const LLBC_TimeSpan LLBC_TimeSpan::zero = LLBC_TimeSpan::FromSeconds(0);
const LLBC_TimeSpan LLBC_TimeSpan::oneSec = LLBC_TimeSpan::FromSeconds(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneMin = LLBC_TimeSpan::FromMinutes(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneHour = LLBC_TimeSpan::FromHours(1);
const LLBC_TimeSpan LLBC_TimeSpan::oneDay = LLBC_TimeSpan::FromDays(1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneSec = LLBC_TimeSpan::FromSeconds(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneMin = LLBC_TimeSpan::FromMinutes(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneHour = LLBC_TimeSpan::FromHours(-1);
const LLBC_TimeSpan LLBC_TimeSpan::negOneDay = LLBC_TimeSpan::FromDays(-1);

LLBC_TimeSpan::LLBC_TimeSpan(const LLBC_String &span)
{
   // Add day part span.
    LLBC_String strippedSpan = span.strip();
    size_t dayIdx = strippedSpan.find(' ');
    if (dayIdx != LLBC_String::npos)
    {
        _span = atoll(strippedSpan.substr(0, dayIdx).c_str()) * LLBC_Time::NumOfMicroSecondsPerDay;
        strippedSpan = strippedSpan.substr(dayIdx + 1);
    }
    else
    {
        _span = 0;
    }

    // Split by ':', fetch hour/minute/second/micro-second parts.
    LLBC_Strings hmsParts = strippedSpan.strip().split(':');
    const LLBC_String &secSpan = hmsParts[hmsParts.size() - 1];
    if (hmsParts.size() >= 2)
    {
        _span += atoll(hmsParts[hmsParts.size() - 2].c_str()) * LLBC_Time::NumOfMicroSecondsPerMinute;
        if (hmsParts.size() >= 3)
            _span += atoll(hmsParts[hmsParts.size() - 3].c_str()) * LLBC_Time::NumOfMicroSecondsPerHour;
    }

    LLBC_Strings secParts = secSpan.strip().split('.', 1);
    _span += atoll(secParts[0].c_str()) * LLBC_Time::NumOfMicroSecondsPerSecond;
    if (secParts.size() == 2)
        _span += atoll(secParts[1].c_str()) * LLBC_Time::NumOfMicroSecondsPerMilliSecond;
}

__LLBC_NS_END
