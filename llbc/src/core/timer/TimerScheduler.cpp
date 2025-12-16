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

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/os/OS_Atomic.h"

#include "llbc/core/timer/Timer.h"
#include "llbc/core/timer/TimerData.h"
#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

sint64 LLBC_TimerScheduler::_maxTimerId = 1;

LLBC_TimerScheduler::LLBC_TimerScheduler()
: _enabled(true)
, _destroying(false)
, _cancelingAll(false)
{
}

LLBC_TimerScheduler::~LLBC_TimerScheduler()
{
    _destroying = true;
    for(auto &elem : _heap)
    {
        LLBC_TimerData *data = elem;
        if (data->unStatus.status.isScheduled)
        {
            llbc_assert(!data->unStatus.status.isHandlingTimeout && !data->unStatus.status.isHandlingCancel &&
                        "Not allow delete <LLBC_TimerScheduler> during timer timeout/cancel handling");

            data->unStatus.status.isScheduled = false;
            data->unStatus.status.isHandlingCancel = true;
            data->timer->OnCancel();
            data->unStatus.status.isHandlingCancel = false;
        }

        if (--data->refCount == 0)
            delete data;
    }
}

LLBC_TimerScheduler::_This *LLBC_TimerScheduler::GetCurrentThreadScheduler()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    return reinterpret_cast<_This *>(tls->coreTls.timerScheduler);
}

void LLBC_TimerScheduler::Update()
{
    if (_enabled == false || _heap.empty())
        return;

    const sint64 now = LLBC_GetMilliseconds();
    do
    {
        // Get top timerData, if not timeout, break.
        LLBC_TimerData *data = _heap.top();
        if (now < data->handle)
            break;

        // Pop top timerData.
        _heap.pop();

        // Process cancelled timerData.
        if (!data->unStatus.status.isScheduled)
        {
            if (--data->refCount == 0)
                delete data;

            continue;
        }

        // Incr refCount && Mark is timeout-handling.
        ++data->refCount;
        data->unStatus.status.isHandlingTimeout = true;
        // Incr has been timeout times.
        ++data->triggeredCount;

        // Call OnTimeout.
        bool reSchedule = true;
        LLBC_Timer *timer = data->timer;
        timer->OnTimeout();

        // If Cancel() or Schedule() called during OnTimeout() call, set reSchedule flag to false.
        if (!data->unStatus.status.isScheduled)
            reSchedule = false;

        // Reach max schedule times, reset <isScheduled> flag && reset reSchedule flag.
        if (data->totalTriggerCount != LLBC_INFINITE &&
            data->triggeredCount >= data->totalTriggerCount)
        {
            data->unStatus.status.isScheduled = false;
            reSchedule = false;
        }

        // Reset isHandlingTimeout flag && Decr refCount.
        data->unStatus.status.isHandlingTimeout = false;
        --data->refCount;

        // Reschedule or try delete timerData.
        if (reSchedule)
        {
            sint64 delay = (data->period != 0) ? (now - data->handle) % data->period : 0;
            data->handle = now + data->period - delay;
            _heap.push(data);
        }
        else
        {
            if (data->refCount == 0 || --data->refCount == 0)
                delete data;
        }
    } while (!_heap.empty());
}

bool LLBC_TimerScheduler::IsEnabled() const
{
    return _enabled;
}

void LLBC_TimerScheduler::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

size_t LLBC_TimerScheduler::GetTimerCount() const
{
    return _heap.size();
}

bool LLBC_TimerScheduler::IsDestroyed() const
{
    return _destroying;
}

int LLBC_TimerScheduler::Schedule(LLBC_Timer *timer,
                                  sint64 firstPeriod,
                                  sint64 period,
                                  size_t triggerCount)
{
    if (UNLIKELY(_destroying))
    {
        LLBC_SetLastError(LLBC_ERROR_TIMER_SCHEDULER_DESTROYING);
        return LLBC_FAILED;
    }

    if (UNLIKELY(_cancelingAll))
    {
        LLBC_SetLastError(LLBC_ERROR_TIMER_SCHEDULER_CANCELING_ALL);
        return LLBC_FAILED;
    }

    auto *data = new LLBC_TimerData;
    data->handle = LLBC_GetMilliseconds() + firstPeriod;
    data->timerId = static_cast <LLBC_TimerId>(LLBC_AtomicFetchAndAdd(&_maxTimerId, 1));
    data->firstPeriod = firstPeriod;
    data->period = period;
    data->totalTriggerCount = triggerCount;
    data->triggeredCount = 0;
    data->timer = timer;
    data->unStatus.statusVal = 0x1; // isScheduled = true
    data->refCount = 2; // LLBC_TimerScheduler:1, LLBC_Timer:1

    if (timer->_timerData)
    {
        if (--timer->_timerData->refCount == 0)
            delete timer->_timerData;
    }

    timer->_timerData = data;
    _heap.push(data);

    return LLBC_OK;
}

int LLBC_TimerScheduler::Cancel(LLBC_Timer *timer)
{
    if (UNLIKELY(_destroying))
    {
        LLBC_SetLastError(LLBC_ERROR_TIMER_SCHEDULER_DESTROYING);
        return LLBC_FAILED;
    }

    LLBC_TimerData *data = timer->_timerData;
    llbc_assert(data->timer == timer && 
                "Timer scheduler internal error, LLBC_TimerData::timer != argument: timer!");

    data->unStatus.status.isScheduled = false;
    data->unStatus.status.isHandlingCancel = true;
    timer->OnCancel();
    data->unStatus.status.isHandlingCancel = false;

    if (data->unStatus.status.isHandlingTimeout)
        return LLBC_OK;

    if (!_cancelingAll &&
        data->handle - LLBC_GetMilliseconds() >= LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME)
    {
        _heap.erase(data);
        if (--data->refCount == 0)
        {
            delete data;
            timer->_timerData = nullptr;
        }
    }

    return LLBC_OK;
}

void LLBC_TimerScheduler::CancelAll()
{
    if (UNLIKELY(_destroying || _cancelingAll))
        return;

    _cancelingAll = true;
    for(auto &elem : _heap)
    {
        LLBC_TimerData *data = elem;
        if (LIKELY(data->unStatus.status.isScheduled))
            data->timer->Cancel();
    }

    _cancelingAll = false;
}

__LLBC_NS_END
