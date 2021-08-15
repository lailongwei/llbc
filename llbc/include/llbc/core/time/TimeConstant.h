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

#ifndef __LLBC_CORE_TIME_TIME_CONSTANT_H__
#define __LLBC_CORE_TIME_TIME_CONSTANT_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The time constant variables define.
 */
class LLBC_EXPORT LLBC_TimeConstant
{
public:
    static const int NumOfSecondsPerDay; // Number of seconds per-day.
    static const int NumOfMilliSecondsPerDay; // Number of milli-seconds per-day.
    static const sint64 NumOfMicroSecondsPerDay; // Number of micro-seconds per-day.
    static const sint64 NumOfNanoSecondsPerDay; // Number of nano-seconds per-day.

    static const int NumOfSecondsPerHour; // Number of seconds per-hour.
    static const int NumOfMilliSecondsPerHour; // Number of milli-seconds per-hour.
    static const sint64 NumOfMicroSecondsPerHour; // Number of micro-seconds per-hour.
    static const sint64 NumOfNanoSecondsPerHour; // Number of nano-seconds per-hour.

    static const int NumOfSecondsPerMinute; // Number of seconds per-minute.
    static const int NumOfMilliSecondsPerMinute; // Number of milli-seconds per-minute.
    static const sint64 NumOfMicroSecondsPerMinute; // Number of micro-seconds per-minute.
    static const sint64 NumOfNanoSecondsPerMinute; // Number of nano-seconds per-minute.

    static const int NumOfMilliSecondsPerSecond; // Number of milli-seconds per-second.
    static const sint64 NumOfMicroSecondsPerSecond; // Number of micro-seconds per-second.
    static const sint64 NumOfNanoSecondsPerSecond; // Number of nano-seconds per-second.

    static const sint64 NumOfMicroSecondsPerMilliSecond; // Number of micro-seconds per-millisecond.
    static const sint64 NumOfNanoSecondsPerMilliSecond; // Number of nano-seconds per-millisecond.

    static const sint64 NumOfNanoSecondsPerMicroSecond; // Number of nano-seconds per-microsecond.
};

__LLBC_NS_END

#endif // !__LLBC_CORE_TIME_TIME_CONSTANT_H__

