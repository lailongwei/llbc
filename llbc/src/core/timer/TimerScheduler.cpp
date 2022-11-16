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
#include "llbc/core/timer/TimerData.h"

#include "llbc/core/timer/TimerScheduler.h"

__LLBC_NS_BEGIN

LLBC_TimerScheduler::LLBC_TimerScheduler()
: _maxTimerId(0)
, _enabled(true)
, _destroyed(false)
{
}

LLBC_TimerScheduler::~LLBC_TimerScheduler()
{
    _destroyed = true;

    const size_t size = _heap.GetSize();
    const _Heap::Container &elems = _heap.GetData();
    for (size_t i = 1; i <= size; ++i)
    {
        LLBC_TimerData *data = const_cast<LLBC_TimerData *>(elems[i]);
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
    if (UNLIKELY(!_enabled))
        return;
    else if (_heap.IsEmpty())
        return;

    LLBC_TimerData *data;
    sint64 now = LLBC_GetMilliSeconds();
    while (_heap.FindTop(data) == LLBC_OK)
    {
        if (now < data->handle)
            break;

        _heap.DeleteTop();
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

            _heap.Insert(data);
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
    return _heap.GetSize();
}

bool LLBC_TimerScheduler::IsDstroyed() const
{
    return _destroyed;
}

int LLBC_TimerScheduler::Schedule(LLBC_Timer *timer, sint64 dueTime, sint64 period)
{
    if (UNLIKELY(_destroyed))
        return LLBC_ERROR_INVALID;

    LLBC_TimerData *data = new LLBC_TimerData;
    memset(data, 0, sizeof(LLBC_TimerData));
    data->handle = LLBC_GetMilliSeconds() + dueTime;
    data->timerId = ++ _maxTimerId;
    data->dueTime = dueTime;
    data->period = period;
    // data->repeatTimes = 0;
    data->timer = timer;
    data->validate = true;
    // data->timeouting = false;
    // data->cancelling = false;
    data->refCount = 2;

    if (timer->_timerData)
    {
        if (--timer->_timerData->refCount == 0)
            delete timer->_timerData;
    }

    timer->_timerData = data;
    _heap.Insert(data);

    return LLBC_OK;
}

int LLBC_TimerScheduler::Cancel(LLBC_Timer *timer)
{
    if (UNLIKELY(_destroyed))
        return LLBC_ERROR_INVALID;

    LLBC_TimerData *data = timer->_timerData;
    ASSERT(data->timer == timer && 
        "Timer scheduler internal error, LLBC_TimerData::timer != argument: timer!");

    data->validate = false;
    data->cancelling = true;
    timer->OnCancel();
    data->cancelling = false;

    if (data->timeouting)
        return LLBC_OK;

    if (data->handle - LLBC_GetMilliSeconds() >= LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME)
    {
        int delElemRet = _heap.DeleteElem(data);
        ASSERT(delElemRet == LLBC_OK &&
            "Timer scheduler internal error, Could not found timer data when Cancel long timeout timer!");
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
    if (UNLIKELY(_destroyed))
        return;

    const size_t size = _heap.GetSize();
    _Heap::Container copyElems(_heap.GetData());
    for (size_t i = 1; i <= size; ++i)
    {
        LLBC_TimerData *data = copyElems[i];
        if (UNLIKELY(!data->validate))
            return;

        data->timer->Cancel();
    }
}

__LLBC_NS_END
