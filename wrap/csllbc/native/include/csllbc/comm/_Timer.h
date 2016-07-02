/**
 * @file    _Timer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/06/29
 * @version 1.0
 *
 * @brief   The csharp timer netive methods encapsulation.
 */
#ifndef __CSLLBC_COMM__TIMER_H__
#define __CSLLBC_COMM__TIMER_H__

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
LLBC_EXTERN_C CSLLBC_EXPORT int csllbc_Timer_Schedule(csllbc_Timer *timer, uint64 dueTime, uint64 period);

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

#endif // !__CSLLBC_COMM__TIMER_H__
