/**
 * @file    Timer.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/1
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/timer/TimerData.h"
#include "llbc/core/timer/Timer.h"
#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

LLBC_Timer::LLBC_Timer(LLBC_Timer::TimeoutFunc timeoutFunc, LLBC_Timer::CancelFunc cancelFunc, LLBC_Timer::Scheduler *scheduler)
: _scheduler(NULL)
, _timerData(NULL)

, _timeoutFunc(timeoutFunc)
, _timeoutDeleg(NULL)

, _cancelFunc(cancelFunc)
, _cancelDeleg(NULL)
{
    if (scheduler)
        _scheduler = scheduler;
    else
        _scheduler = reinterpret_cast<
            Scheduler *>(__LLBC_GetLibTls()->coreTls.timerScheduler);
}

LLBC_Timer::LLBC_Timer(LLBC_IDelegate1<LLBC_Timer *> *timeoutDeleg, LLBC_IDelegate1<LLBC_Timer *> *cancelDeleg, LLBC_Timer::Scheduler *scheduler)
: _scheduler(NULL)
, _timerData(NULL)

, _timeoutFunc(NULL)
, _timeoutDeleg(timeoutDeleg)

, _cancelFunc(NULL)
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

    _timeoutFunc = NULL;
    LLBC_XDelete(_timeoutDeleg);

    _cancelFunc = NULL;
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

void LLBC_Timer::OnTimeout()
{
    _timeoutDeleg ? _timeoutDeleg->Invoke(this) : (*_timeoutFunc)(this);
}

void LLBC_Timer::OnCancel()
{
    if (_cancelDeleg)
        _cancelDeleg->Invoke(this);
    else if (_cancelFunc)
        (*_cancelFunc)(this);
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

    if (UNLIKELY(dueTime < 0 || period < 0))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    period = period == 0 ? dueTime : period;
    return _scheduler->Schedule(this, dueTime, period);
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
