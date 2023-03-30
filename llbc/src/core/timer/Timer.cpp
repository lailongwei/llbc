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

#include "llbc/core/timer/TimerData.h"
#include "llbc/core/timer/Timer.h"
#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

LLBC_Timer::LLBC_Timer(const LLBC_Delegate<void(LLBC_Timer *)> &timeoutDeleg,
                       const LLBC_Delegate<void(LLBC_Timer *)> &cancelDeleg,
                       LLBC_Timer::Scheduler *scheduler)
: _scheduler(scheduler)
, _timerData(nullptr)

, _data(nullptr)
, _timeoutDeleg(timeoutDeleg)
, _cancelDeleg(cancelDeleg)
{
}

LLBC_Timer::~LLBC_Timer()
{
    if (_timerData)
    {
        Cancel();
        if (--_timerData->refCount == 0)
            delete _timerData;
;
    }

    if (_data)
        delete _data;
}

uint64 LLBC_Timer::GetDueTime() const
{
    return _timerData ? _timerData->dueTime : 0;
}

uint64 LLBC_Timer::GetPeriod() const
{
    return _timerData ? _timerData->period : 0;
}

LLBC_TimerId LLBC_Timer::GetTimerId() const
{
    return _timerData ? _timerData->timerId : LLBC_INVALID_TIMER_ID;
}

void LLBC_Timer::SetTimeoutHandler(const LLBC_Delegate<void(LLBC_Timer *)> &timeoutDeleg)
{
    _timeoutDeleg = timeoutDeleg;
}

void LLBC_Timer::SetCancelHandler(const LLBC_Delegate<void(LLBC_Timer *)> &cancelDeleg)
{
    _cancelDeleg = cancelDeleg;
}

LLBC_Variant &LLBC_Timer::GetTimerData()
{
    if (!_data)
        _data = new LLBC_Variant();
    return *_data;
}

const LLBC_Variant & LLBC_Timer::GetTimerData() const
{
    return const_cast<LLBC_Timer *>(this)->GetTimerData();
}

void LLBC_Timer::OnTimeout()
{
    if (LIKELY(_timeoutDeleg))
        _timeoutDeleg(this);
}

void LLBC_Timer::OnCancel()
{
    if (_cancelDeleg)
        _cancelDeleg(this);
}

int LLBC_Timer::Schedule(const LLBC_TimeSpan &dueTime, const LLBC_TimeSpan &period)
{
    // Note: Allow reschedule in <OnCancel> event meth.
    // if (_timerData && _timerData->cancelling)
    // {
    //     LLBC_SetLastError(LLBC_ERROR_ILLEGAL);
    //     return LLBC_FAILED;
    // }

    const int cancelRet = Cancel();
    if (UNLIKELY(cancelRet != LLBC_OK))
        return cancelRet;

    if (UNLIKELY(!_scheduler))
    {
        _scheduler = reinterpret_cast<Scheduler *>(__LLBC_GetLibTls()->coreTls.timerScheduler);
        if (UNLIKELY(!_scheduler))
        {
            LLBC_SetLastError(LLBC_ERROR_INVALID);
            return LLBC_FAILED;
        }
    }

    const sint64 dueTimeMillis = MAX(0ll, dueTime.GetTotalMilliSeconds());
    sint64 periodMillis = MAX(0ll, period.GetTotalMilliSeconds());
    if (periodMillis == 0ll)
        periodMillis = dueTimeMillis;

    return _scheduler->Schedule(this, dueTimeMillis, periodMillis);
}

int LLBC_Timer::Cancel()
{
    if (!_timerData || !_timerData->validate)
        return LLBC_OK;

    if (UNLIKELY(!_scheduler))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    return _scheduler->Cancel(this);
}

bool LLBC_Timer::IsScheduling() const
{
    return (_timerData != nullptr && _timerData->validate);
}

bool LLBC_Timer::IsTimeouting() const
{
    return _timerData ? _timerData->timeouting : false;
}

bool LLBC_Timer::IsCancelling() const
{
    return _timerData ? _timerData->cancelling: false;
}

LLBC_String LLBC_Timer::ToString() const
{
    return LLBC_String().format(
        "timerId: %llu, dueTime: %llu, period: %llu, scheduling: %s",
        GetTimerId(), GetDueTime(), GetPeriod(), IsScheduling()? "true" : "false");
}

__LLBC_NS_END
