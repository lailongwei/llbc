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

#include "core/Example_Core_Time.h"

int Example_Core_Time::Run(int argc, char *argv[])
{
    // ========================= Get current time =========================
    // LLBC_Time::Now() returns the current local time object.
    LLBC_Time now = LLBC_Time::Now();
    LLBC_PrintLn("Current time (now): %s", now.ToString().c_str());
    LLBC_PrintLn("- Timestamp (seconds): %lld", static_cast<long long>(now.GetTimestampInSecs()));
    LLBC_PrintLn("- Timestamp (milliseconds): %lld", static_cast<long long>(now.GetTimestampInMillis()));

    // ========================= Formatting =========================
    // Format() uses the default format (local timezone), FormatAsGmt() uses UTC; a strftime-style format string may also be passed.
    LLBC_PrintLn("Default format (local): %s", now.Format().c_str());
    LLBC_PrintLn("Default format (UTC):   %s", now.FormatAsGmt().c_str());
    LLBC_PrintLn("Custom format (%%Y-%%m-%%d): %s", now.Format("%Y-%m-%d").c_str());

    // Extract each time component.
    LLBC_PrintLn("Year:%d Month:%d Day:%d Hour:%d Minute:%d Second:%d",
                 now.GetYear(), now.GetMonth(), now.GetDayOfMonth(),
                 now.GetHour(), now.GetMinute(), now.GetSecond());

    // ========================= TimeSpan (time span) =========================
    // Prefer factory methods to construct a TimeSpan rather than parsing strings by hand.
    LLBC_TimeSpan oneDay = LLBC_TimeSpan::FromDays(1);
    LLBC_TimeSpan halfHour = LLBC_TimeSpan::FromMinutes(30);
    LLBC_TimeSpan combined = LLBC_TimeSpan::FromHours(1, 2, 3); // 1h2m3s
    LLBC_PrintLn("oneDay: %s", oneDay.ToString().c_str());
    LLBC_PrintLn("halfHour: %s", halfHour.ToString().c_str());
    LLBC_PrintLn("FromHours(1,2,3): %s, total seconds:%d",
                 combined.ToString().c_str(), combined.GetTotalSeconds());

    // ========================= Arithmetic with Time and TimeSpan =========================
    // Time +/- TimeSpan yields a new Time; Time - Time yields a TimeSpan.
    LLBC_Time tomorrow = now + oneDay;
    LLBC_Time yesterday = now - oneDay;
    LLBC_PrintLn("Same time tomorrow: %s", tomorrow.ToString().c_str());
    LLBC_PrintLn("Same time yesterday: %s", yesterday.ToString().c_str());
    LLBC_PrintLn("Difference between tomorrow and yesterday: %s", (tomorrow - yesterday).ToString().c_str());

    // The AddXXX family returns a new Time object (the original is not modified).
    LLBC_PrintLn("now.AddHours(8): %s", now.AddHours(8).ToString().c_str());
    LLBC_PrintLn("now.AddDays(-3): %s", now.AddDays(-3).ToString().c_str());

    // ========================= Parsing =========================
    // FromTimeStr() parses a Time from a string; FromSpanStr() parses a TimeSpan from a string.
    LLBC_Time parsedTime = LLBC_Time::FromTimeStr("2025-07-01 20:05:08");
    LLBC_PrintLn("FromTimeStr(\"2025-07-01 20:05:08\"): %s", parsedTime.ToString().c_str());

    LLBC_TimeSpan parsedSpan = LLBC_TimeSpan::FromSpanStr("05 01:02:03"); // 5d 1h2m3s
    LLBC_PrintLn("FromSpanStr(\"05 01:02:03\"): %s, total days:%d",
                 parsedSpan.ToString().c_str(), parsedSpan.GetTotalDays());

    return LLBC_OK;
}
