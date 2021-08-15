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

#include "llbc/core/time/TimeConstant.h"

__LLBC_NS_BEGIN

const int LLBC_TimeConstant::NumOfSecondsPerDay = 86400;
const int LLBC_TimeConstant::NumOfMilliSecondsPerDay = 86400000;
#if LLBC_TARGET_PLATFORM_WIN32
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfMicroSecondsPerDay = 86400000000I64;
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerDay = 86400000000000I64;
#else
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfMicroSecondsPerDay = 86400000000LL;
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerDay = 86400000000000LL;
#endif

LLBC_EXPORT const int LLBC_TimeConstant::NumOfSecondsPerHour = 3600;
LLBC_EXPORT const int LLBC_TimeConstant::NumOfMilliSecondsPerHour = 3600000;
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfMicroSecondsPerHour = 3600000000;
#if LLBC_TARGET_PLATFORM_WIN32
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerHour = 3600000000000I64;
#else
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerHour = 3600000000000LL;
#endif

LLBC_EXPORT const int LLBC_TimeConstant::NumOfSecondsPerMinute = 60;
LLBC_EXPORT const int LLBC_TimeConstant::NumOfMilliSecondsPerMinute = 60000;
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfMicroSecondsPerMinute = 60000000;
#if LLBC_TARGET_PLATFORM_WIN32
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerMinute = 60000000000I64;
#else
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerMinute = 60000000000LL;
#endif

LLBC_EXPORT const int LLBC_TimeConstant::NumOfMilliSecondsPerSecond = 1000;
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfMicroSecondsPerSecond = 1000000;
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerSecond = 1000000000;

LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfMicroSecondsPerMilliSecond = 1000;
LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerMilliSecond = 1000000;

LLBC_EXPORT const sint64 LLBC_TimeConstant::NumOfNanoSecondsPerMicroSecond = 1000;

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"

