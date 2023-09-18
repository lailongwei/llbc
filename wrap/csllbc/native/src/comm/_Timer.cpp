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

#include "csllbc/common/Export.h"

#include "csllbc/comm/csTimer.h"
#include "csllbc/comm/_Timer.h"

LLBC_BEGIN_C_DECL

csllbc_Timer *csllbc_Timer_Create(csllbc_Delegates::Deleg_Timer_OnTimeout timeoutDeleg,
                                  csllbc_Delegates::Deleg_Timer_OnCancel cancelDeleg)
{
    return new csllbc_Timer(timeoutDeleg, cancelDeleg);
}

void csllbc_Timer_Destroy(csllbc_Timer *timer)
{
    delete timer;
}

int csllbc_Timer_Schedule(csllbc_Timer *timer, sint64 dueTime, sint64 period)
{
    return timer->Schedule(LLBC_TimeSpan::FromMillis(dueTime),
                           LLBC_TimeSpan::FromMillis(period));
}

int csllbc_Timer_IsScheduling(csllbc_Timer *timer)
{
    return static_cast<int>(timer->IsScheduling());
}

void csllbc_Timer_Cancel(csllbc_Timer *timer)
{
    timer->Cancel();
}

uint64 csllbc_Timer_GetDueTime(csllbc_Timer *timer)
{
    return timer->GetDueTime();
}

uint64 csllbc_Timer_GetPeriod(csllbc_Timer *timer)
{
    return timer->GetPeriod();
}

uint64 csllbc_Timer_GetTimerId(csllbc_Timer *timer)
{
    return timer->GetTimerId();
}

LLBC_END_C_DECL