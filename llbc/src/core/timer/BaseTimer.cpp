/**
 * @file    BaseTimer.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/1
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/timer/BaseTimer.h"
#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

LLBC_BaseTimer::LLBC_BaseTimer(LLBC_BaseTimer::Scheduler *scheduler)
: _timerId(LLBC_INVALID_TIMER_ID)

, _dueTime(0)
, _period(0)

, _scheduling(false)
{
    if (scheduler)
    {
        _scheduler = scheduler;
    }
    else
    {
        __LLBC_LibTls *tls = __LLBC_GetLibTls();
        _scheduler = reinterpret_cast<
            Scheduler *>(tls->coreTls.timerScheduler);
    }
}

LLBC_BaseTimer::~LLBC_BaseTimer()
{
    this->Cancel();
}

uint64 LLBC_BaseTimer::GetDueTime() const
{
    return _dueTime;
}

uint64 LLBC_BaseTimer::GetPeriod() const
{
    return _period;
}

LLBC_TimerId LLBC_BaseTimer::GetTimerId() const
{
    return _timerId;
}

int LLBC_BaseTimer::Schedule(uint64 dueTime, uint64 period)
{
    if (_scheduling)
    {
        LLBC_SetLastError(LLBC_ERROR_REENTRY);
        return LLBC_RTN_FAILED;
    }

    if (UNLIKELY(!_scheduler || _scheduler->IsDstroyed()))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_RTN_FAILED;
    }

    _dueTime = dueTime;
    _period = period;

    return _scheduler->Schedule(this);
}

int LLBC_BaseTimer::Cancel()
{
    if (!_scheduling)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    if (UNLIKELY(!_scheduler || _scheduler->IsDstroyed()))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_RTN_FAILED;
    }

    return _scheduler->Cancel(this);
}

bool LLBC_BaseTimer::IsScheduling() const
{
    return _scheduling;
}

LLBC_String LLBC_BaseTimer::ToString() const
{
    return LLBC_String().format("timerId: %llu, dueTime: %llu, period: %llu, scheduling: %s",
        _timerId, _dueTime, _period, _scheduling ? "true" : "false");
}

 void LLBC_BaseTimer::SetScheduling(bool scheduling)
{
    _scheduling = scheduling;
}

void LLBC_BaseTimer::SetTimerId(LLBC_TimerId timerId)
{
    _timerId = timerId;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
