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

__LLBC_NS_END
