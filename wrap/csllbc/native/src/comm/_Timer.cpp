/**
 * @file    _Timer.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/06/29
 * @version 1.0
 *
 * @brief   The csharp timer netive methods encapsulation.
 */

#include "csllbc/common/Export.h"

#include "csllbc/comm/csTimer.h"
#include "csllbc/comm/_Timer.h"

LLBC_BEGIN_C_DECL

csllbc_Timer *csllbc_Timer_Create(csllbc_Delegates::Deleg_Timer_OnTimeout timeoutDeleg,
                                  csllbc_Delegates::Deleg_Timer_OnCancel cancelDeleg)
{
    return LLBC_New2(csllbc_Timer, timeoutDeleg, cancelDeleg);
}

void csllbc_Timer_Destroy(csllbc_Timer *timer)
{
    LLBC_Delete(timer);
}

int csllbc_Timer_Schedule(csllbc_Timer *timer, uint64 dueTime, uint64 period)
{
    return timer->Schedule(dueTime, period);
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