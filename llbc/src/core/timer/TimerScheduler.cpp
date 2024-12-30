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

#include "llbc/core/timer/Timer.h"
#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

LLBC_TimerScheduler::LLBC_TimerScheduler()
: _maxTimerId(0)
, _enabled(true)
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
        if (data->validate)
        {
            data->validate = false;
            data->cancelling = true;
            data->timer->OnCancel();
            data->cancelling = false;
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
    LLBC_ReturnIf(_enabled == false || _heap.empty(), void());

    sint64 now = LLBC_GetMilliseconds();
    while (_heap.empty() == false)
    {
        LLBC_TimerData *data = _heap.top();
        if(data == nullptr)
        {
            _heap.pop();
            continue;
        }

        if (now < data->handle)
            break;

        _heap.pop();
        if (!data->validate)
        {
            if (--data->refCount == 0)
                delete data;

            continue;
        }

        data->timeouting = true;

        bool reSchedule = true;
        LLBC_Timer *timer = data->timer;
#if LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
        sint64 pseudoNow = now;
        while (pseudoNow >= data->handle)
#endif // LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
        {
            ++data->repeatTimes;
            timer->OnTimeout();

            // Cancel() or Schedule() called.
            if (!data->validate)
            {
                reSchedule = false;
#if LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
                break;
#endif // LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
            }

#if LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
            if (data->period == 0)
                break;

            if (UNLIKELY(pseudoNow < data->period))
                break;

            pseudoNow -= data->period;
#endif // LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
        }

        data->timeouting = false;
        if (reSchedule)
        {
            sint64 delay = (data->period != 0) ? (now - data->handle) % data->period : 0;
            data->handle = now + data->period - delay;
            _heap.push(data);
        }
        else
        {
            if (--data->refCount == 0)
                delete data;
        }
    }
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

int LLBC_TimerScheduler::Schedule(LLBC_Timer *timer, sint64 dueTime, sint64 period)
{
    if (UNLIKELY(_destroying))
    {
        LLBC_SetLastError(LLBC_ERROR_TIMER_SCHEDULER_DESTROYING);
        return LLBC_FAILED;
    }
    else if (UNLIKELY(_cancelingAll))
    {
        LLBC_SetLastError(LLBC_ERROR_TIMER_SCHEDULER_CANCELING_ALL);
        return LLBC_FAILED;
    }

    auto *data = new LLBC_TimerData;
    memset(data, 0, sizeof(LLBC_TimerData));
    data->handle = LLBC_GetMilliseconds() + dueTime;
    data->timerId = ++_maxTimerId;
    data->dueTime = dueTime;
    data->period = period;
    data->timer = timer;
    data->validate = true;
    data->refCount = 2;

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
    ASSERT(data->timer == timer && 
        "Timer scheduler internal error, LLBC_TimerData::timer != argument: timer!");

    data->validate = false;
    data->cancelling = true;
    timer->OnCancel();
    data->cancelling = false;

    if (data->timeouting)
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
        if (LIKELY(data->validate))
            data->timer->Cancel();
    }

    _cancelingAll = false;
}

__LLBC_NS_END
