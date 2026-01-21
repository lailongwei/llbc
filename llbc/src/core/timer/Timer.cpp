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

#include "llbc/core/time/Time.h"
#include "llbc/core/variant/Variant.h"

#include "llbc/core/timer/Timer.h"
#include "llbc/core/timer/TimerData.h"
#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

const LLBC_Delegate<void(LLBC_Timer *)> LLBC_Timer::_invalidCancelDeleg;

LLBC_Timer::LLBC_Timer(const LLBC_Delegate<void(LLBC_Timer *)> &timeoutDeleg,
                       const LLBC_Delegate<void(LLBC_Timer *)> &cancelDeleg,
                       LLBC_TimerScheduler *scheduler)
: _scheduler(scheduler)
, _timerData(nullptr)

, _data(nullptr)
, _timeoutDeleg(timeoutDeleg)
, _cancelDeleg(cancelDeleg ? new LLBC_Delegate<void(LLBC_Timer *)>(cancelDeleg) : nullptr)
{
}

LLBC_Timer::~LLBC_Timer()
{
    if (_timerData)
    {
        Cancel();
        if (--_timerData->refCount == 0)
            delete _timerData;
    }

    if (_data)
        delete _data;

    if (_cancelDeleg)
        delete _cancelDeleg;
}

LLBC_TimerId LLBC_Timer::GetTimerId() const
{
    return _timerData ? _timerData->timerId : LLBC_INVALID_TIMER_ID;
}

LLBC_TimeSpan LLBC_Timer::GetFirstPeriod() const
{
    return _timerData ?
        LLBC_TimeSpan::FromMillis(_timerData->firstPeriod) : LLBC_TimeSpan::zero;
}

LLBC_TimeSpan LLBC_Timer::GetPeriod() const
{
    return _timerData ?
        LLBC_TimeSpan::FromMillis(_timerData->period) : LLBC_TimeSpan::zero;
}

size_t LLBC_Timer::GetTotalTriggerCount() const
{
    return _timerData ? _timerData->totalTriggerCount : LLBC_INFINITE;
}

size_t LLBC_Timer::GetTriggeredCount() const
{
    return _timerData ? _timerData->triggeredCount : 0;
}

LLBC_Variant &LLBC_Timer::GetTimerData()
{
    return *(_data ? _data : (_data = new LLBC_Variant));
}

const LLBC_Variant &LLBC_Timer::GetTimerData() const
{
    return _data ? *_data : LLBC_Variant::nil;
}

LLBC_Time LLBC_Timer::GetTimeoutTime() const
{
    return _timerData ?
        LLBC_Time::FromMillis(_timerData->handle) :
            LLBC_Time::utcBegin;
}

int LLBC_Timer::Schedule(const LLBC_TimeSpan &firstPeriod,
                         const LLBC_TimeSpan &period,
                         size_t triggerCount)
{
    // Note: Allow reschedule in <OnCancel> event meth.
    // if (_timerData && _timerData->unFlags.flags.isHandlingCancel)
    // {
    //     LLBC_SetLastError(LLBC_ERROR_ILLEGAL);
    //     return LLBC_FAILED;
    // }

    const int cancelRet = Cancel();
    if (UNLIKELY(cancelRet != LLBC_OK))
        return cancelRet;

    if (UNLIKELY(!_scheduler))
    {
        _scheduler = reinterpret_cast<LLBC_TimerScheduler *>(
            __LLBC_GetLibTls()->coreTls.timerScheduler);
        if (UNLIKELY(!_scheduler))
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
    }

    const sint64 firstPeriodInMillis = MAX(0ll, firstPeriod.GetTotalMillis());
    sint64 periodMillis = MAX(0ll, period.GetTotalMillis());
    if (periodMillis == 0ll)
        periodMillis = firstPeriodInMillis;

    if (UNLIKELY(triggerCount == 0))
        triggerCount = 1;

    return _scheduler->Schedule(this, firstPeriodInMillis, periodMillis, triggerCount);
}

int LLBC_Timer::Cancel()
{
    // Make sure timer scheduled.
    if (!_timerData ||
        !_timerData->unStatus.status.isScheduled ||
        !_scheduler)
        return LLBC_OK;

    // Not allow call Cancel() during cancel handling.
    if (UNLIKELY(_timerData->unStatus.status.isHandlingCancel))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    // Cancel timer in timer scheduler.
    return _scheduler->Cancel(this);
}

bool LLBC_Timer::IsScheduled() const
{
    return (_timerData && _timerData->unStatus.status.isScheduled);
}

bool LLBC_Timer::IsHandlingTimeout() const
{
    return _timerData ? _timerData->unStatus.status.isHandlingTimeout: false;
}

bool LLBC_Timer::IsHandlingCancel() const
{
    return _timerData ? _timerData->unStatus.status.isHandlingCancel: false;
}

LLBC_String LLBC_Timer::ToString() const
{
    return LLBC_String().format(
        "LLBC_Timer(%llu, firstPeriod:%llu ms, period:%llu ms, "
        "trigger:%zu/%lld, status:[scheduled:%s handlingTimeout:%s handlingCancel:%s])",
        GetTimerId(),
        GetFirstPeriod().GetTotalMillis(),
        GetPeriod().GetTotalMillis(),
        GetTriggeredCount(),
        GetTotalTriggerCount() == static_cast<size_t>(LLBC_INFINITE) ?
            -1ll :
                static_cast<sint64>(GetTotalTriggerCount()),
        IsScheduled() ? "true" : "false",
        IsHandlingTimeout() ? "true" : "false",
        IsHandlingCancel() ? "true" : "false");
}

__LLBC_NS_END
