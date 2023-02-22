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

#include "csllbc/common/Common.h"
#include "csllbc/core/Core.h"

class csllbc_Timer;

/**
 * Create new csharp layer native timer class.
 */
LLBC_EXTERN_C CSLLBC_EXPORT csllbc_Timer *csllbc_Timer_Create(csllbc_Delegates::Deleg_Timer_OnTimeout timeoutDeleg,
                                                              csllbc_Delegates::Deleg_Timer_OnCancel cancelDeleg);

/**
 * Destroy csharp layer native timer class.
 */
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Timer_Destroy(csllbc_Timer *timer);

/**
 * Schedule timer.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Timer_Schedule(csllbc_Timer *timer, sint64 dueTime, sint64 period);

/**
 * Check given timer in scheduling or not.
 */
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Timer_IsScheduling(csllbc_Timer *timer);

/**
 * Cancel timer.
 */
LLBC_EXTERN_C CSLLBC_EXPORT void csllbc_Timer_Cancel(csllbc_Timer *timer);

/**
 * Get timer duetime and period.
 */
LLBC_EXTERN_C CSLLBC_EXPORT uint64 csllbc_Timer_GetDueTime(csllbc_Timer *timer);
LLBC_EXTERN_C CSLLBC_EXPORT uint64 csllbc_Timer_GetPeriod(csllbc_Timer *timer);

/**
 * Get timer id(per-service global unique).
 */
LLBC_EXTERN_C CSLLBC_EXPORT uint64 csllbc_Timer_GetTimerId(csllbc_Timer *timer);


