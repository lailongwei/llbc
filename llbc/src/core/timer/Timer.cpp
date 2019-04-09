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

#include "llbc/core/time/Common.h"
#include "llbc/core/variant/Common.h"

#include "llbc/core/timer/TimerData.h"
#include "llbc/core/timer/Timer.h"
#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

LLBC_Timer::LLBC_Timer(LLBC_IDelegate1<void, LLBC_Timer *> *timeoutDeleg,
                       LLBC_IDelegate1<void, LLBC_Timer *> *cancelDeleg,
                       LLBC_Timer::Scheduler *scheduler)
: _scheduler(NULL)
, _timerData(NULL)

, _data(NULL)
, _timeoutDeleg(timeoutDeleg)
, _cancelDeleg(cancelDeleg)
{
    if (scheduler)
        _scheduler = scheduler;
    else
        _scheduler = reinterpret_cast<
            Scheduler *>(__LLBC_GetLibTls()->coreTls.timerScheduler);
}
LLBC_Timer::~LLBC_Timer()
{
    if (_timerData)
    {
        Cancel();
        if (--_timerData->refCount == 0)
            LLBC_Delete(_timerData);

        _timerData = NULL;
    }

    LLBC_XDelete(_data);
    LLBC_XDelete(_timeoutDeleg);
    LLBC_XDelete(_cancelDeleg);

    _scheduler = NULL;
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

void LLBC_Timer::SetTimeoutHandler(void (*timeoutFunc)(LLBC_Timer *))
{
    typedef LLBC_Func1<void, LLBC_Timer *> __TimeoutFuncDeleg;

    LLBC_XDelete(_timeoutDeleg);
    if (timeoutFunc != NULL)
        _timeoutDeleg = LLBC_New1(__TimeoutFuncDeleg, timeoutFunc);
}

void LLBC_Timer::SetTimeoutHandler(LLBC_IDelegate1<void, LLBC_Timer *> *timeoutDeleg)
{
    if (UNLIKELY(timeoutDeleg == _timeoutDeleg))
        return;

    LLBC_XDelete(_timeoutDeleg);
    _timeoutDeleg = timeoutDeleg;
}

void LLBC_Timer::SetCancelHandler(void (*cancelFunc)(LLBC_Timer *))
{
    typedef LLBC_Func1<void, LLBC_Timer *> __CancelFuncDeleg;

    LLBC_XDelete(_cancelDeleg);
    if (cancelFunc != NULL)
        _cancelDeleg = LLBC_New1(__CancelFuncDeleg, cancelFunc);
}

void LLBC_Timer::SetCancelHandler(LLBC_IDelegate1<void, LLBC_Timer *> *cancelDeleg)
{
    if (UNLIKELY(cancelDeleg == _cancelDeleg))
        return;

    LLBC_XDelete(_cancelDeleg);
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
        _timeoutDeleg->Invoke(this);
}

void LLBC_Timer::OnCancel()
{
    if (_cancelDeleg)
        _cancelDeleg->Invoke(this);
}

int LLBC_Timer::Schedule(uint64 dueTime, uint64 period)
{
    if (_timerData && _timerData->cancelling)
    {
        LLBC_SetLastError(LLBC_ERROR_ILLEGAL);
        return LLBC_FAILED;
    }

    int cancelRet = Cancel();
    if (UNLIKELY(cancelRet!= LLBC_OK))
        return cancelRet;

    if (UNLIKELY(!_scheduler))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    period = period == 0 ? dueTime : period;
    return _scheduler->Schedule(this, dueTime, period);
}

int LLBC_Timer::Schedule(const LLBC_Time &dueTime, uint64 period)
{
    const LLBC_TimeSpan span = dueTime - LLBC_Time::Now();
    return Schedule(LIKELY(span.GetTotalMilliSeconds() >= 0) ? 
        static_cast<uint64>(span.GetTotalMilliSeconds()) : 0, period);
}

int LLBC_Timer::Schedule(const LLBC_TimeSpan &dueTime)
{
    return Schedule(LIKELY(dueTime.GetTotalMilliSeconds() >= 0) ?
        static_cast<uint64>(dueTime.GetTotalMilliSeconds()) : 0, 0);
}

int LLBC_Timer::Schedule(const LLBC_TimeSpan &dueTime, const LLBC_TimeSpan &period)
{
    return Schedule(LIKELY(dueTime.GetTotalMilliSeconds() >= 0) ?
            static_cast<uint64>(dueTime.GetTotalMilliSeconds()) : 0, 
                    LIKELY(period.GetTotalMilliSeconds() >= 0) ? 
            static_cast<uint64>(period.GetTotalMilliSeconds()) : 0);
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
    return (_timerData != NULL && _timerData->validate);
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

#include "llbc/common/AfterIncl.h"
