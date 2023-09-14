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

#pragma once

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The time constant variables define.
 */
class LLBC_TimeConst
{
public:
    static constexpr int NumOfSecondsPerDay = 86400; // Number of seconds per-day.
    static constexpr int NumOfMillisPerDay = 86400000; // Number of milli-seconds per-day.
    #if LLBC_TARGET_PLATFORM_WIN32
    static constexpr sint64 NumOfMicrosPerDay = 86400000000I64; // Number of micro-seconds per-day.
    static constexpr sint64 NumOfNanosPerDay = 86400000000000I64; // Number of nano-seconds per-day.
    #else
    static constexpr sint64 NumOfMicrosPerDay = 86400000000LL; // Number of micro-seconds per-day.
    static constexpr sint64 NumOfNanosPerDay = 86400000000000LL; // Number of nano-seconds per-day.
    #endif

    static constexpr int NumOfSecondsPerHour = 3600; // Number of seconds per-hour.
    static constexpr int NumOfMillisPerHour = 3600000; // Number of milli-seconds per-hour.
    static constexpr sint64 NumOfMicrosPerHour = 3600000000; // Number of micro-seconds per-hour.
    #if LLBC_TARGET_PLATFORM_WIN32
    static constexpr sint64 NumOfNanosPerHour = 3600000000000I64; // Number of nano-seconds per-hour.
    #else
    static constexpr sint64 NumOfNanosPerHour = 3600000000000LL; // Number of nano-seconds per-hour.
    #endif

    static constexpr int NumOfSecondsPerMinute = 60; // Number of seconds per-minute.
    static constexpr int NumOfMillisPerMinute = 60000; // Number of milli-seconds per-minute.
    static constexpr sint64 NumOfMicrosPerMinute = 60000000; // Number of micro-seconds per-minute.
    #if LLBC_TARGET_PLATFORM_WIN32
    static constexpr sint64 NumOfNanosPerMinute = 60000000000I64; // Number of nano-seconds per-minute.
    #else
    static constexpr sint64 NumOfNanosPerMinute = 60000000000LL; // Number of nano-seconds per-minute.
    #endif

    static constexpr int NumOfMillisPerSecond = 1000; // Number of milli-seconds per-second.
    static constexpr sint64 NumOfMicrosPerSecond = 1000000; // Number of micro-seconds per-second.
    static constexpr sint64 NumOfNanosPerSecond = 1000000000; // Number of nano-seconds per-second.

    static constexpr sint64 NumOfMicrosPerMillisecond = 1000; // Number of micro-seconds per-millisecond.
    static constexpr sint64 NumOfNanosPerMillisecond = 1000000; // Number of nano-seconds per-millisecond.

    static constexpr sint64 NumOfNanosPerMicrosecond = 1000; // Number of nano-seconds per-microsecond.
};

__LLBC_NS_END
