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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/time/TimeSpan.h"

__LLBC_NS_BEGIN

LLBC_TimeSpan::LLBC_TimeSpan(const LLBC_String &span)
{

    // Ensure the span string is time format, not datetime format.
    LLBC_String spanRepr = span;
    LLBC_String::size_type spaceIdx = span.find(' ');
    if (spaceIdx != LLBC_String::npos)
        spanRepr = span.substr(spaceIdx + 1);

    // If the span string is empty, set span value to 0.
    if (spanRepr.empty())
    {
        _span = 0;
        return;
    }

    // Split by ':', fetch hour,minute,second, millosecond parts.
    LLBC_Strings spanParts = span.split(':');
    if (spanParts.size() == 1) // Only has second part.
    {
        spanParts.insert(spanParts.begin(), "0");
        spanParts.insert(spanParts.begin(), "0");
    }
    else if (spanParts.size() == 2) // Only has second and minute parts.
    {
        spanParts.insert(spanParts.begin(), "0");
    }

    _span = LLBC_Str2Int32(spanParts[0].c_str()) * LLBC_Time::NumOfSecondsPerHour +
        LLBC_Str2Int32(spanParts[1].c_str()) * LLBC_Time::NumOfSecondsPerMinute;

    LLBC_Strings secParts = spanParts[2].split('.', 1);
    _span += LLBC_Str2Int32(secParts[0].c_str());
    if (secParts.size() == 2)
        _span += LLBC_Str2Int32(secParts[1].c_str()) / static_cast<double>(LLBC_Time::NumOfMicroSecondsPerSecond);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
