/**
 * @file    TimerScheduler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/12/01
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Time.h"

#include "llbc/core/timer/BaseTimer.h"
#include "llbc/core/timer/TimerData.h"

#include "llbc/core/timer/TimerScheduler.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_TimerScheduler *__g_entryThreadTimerScheduler = NULL;

__LLBC_INTERNAL_NS_END

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

    size_t size = _heap.GetSize();
    const _Heap::Container &elems = _heap.GetData();
    for (size_t i = 1; i <= size; i++)
    {
        LLBC_TimerData *data = const_cast<LLBC_TimerData *>(elems[i]);
        if (data->validate)
        {
            data->validate = false;
            data->timer->SetScheduling(false);

            data->timer->OnCancel();
        }
    }

    for (size_t i = 1; i < size; i++)
    {
        delete const_cast<LLBC_TimerData *>(elems[i]);
    }
}

void LLBC_TimerScheduler::CreateEntryThreadScheduler()
{
    LLBC_TimerScheduler *&scheduler = 
        LLBC_INTERNAL_NS __g_entryThreadTimerScheduler;
    if (!scheduler)
    {
        scheduler = new LLBC_TimerScheduler;
    }
}

void LLBC_TimerScheduler::DestroyEntryThreadScheduler()
{
    LLBC_XDelete(LLBC_INTERNAL_NS __g_entryThreadTimerScheduler);
}

LLBC_TimerScheduler::_This *LLBC_TimerScheduler::GetEntryThreadScheduler()
{
    return LLBC_INTERNAL_NS __g_entryThreadTimerScheduler;
}

LLBC_TimerScheduler::_This *LLBC_TimerScheduler::GetCurrentThreadScheduler()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    return reinterpret_cast<_This *>(tls->coreTls.timerScheduler);
}

void LLBC_TimerScheduler::Update()
{
    if (!_enabled)
        return;

    LLBC_TimerData *data;
    uint64 now = LLBC_GetMilliSeconds();

    while (_heap.FindTop(data) == LLBC_OK)
    {
        if (now < data->handle)
            break;

        _heap.DeleteTop();

        if (!data->validate)
        {
            _idxMap.erase(data->timerId);
            delete data;
            continue;
        }

        data->validate = false;
        data->timer->SetScheduling(false);

        bool reSchedule = false;
#if LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
        uint64 pseudoNow = now;
        while (pseudoNow >= data->handle)
#endif // LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
        {
            ++ data->repeatTimes;
            reSchedule = data->timer->OnTimeout();
#if LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE
            if (!reSchedule)
                break;
#endif // !LLBC_CFG_CORE_TIMER_STRICT_SCHEDULE

            if (data->timer->IsScheduling())
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

        if (reSchedule)
        {
            uint64 delay = (data->period != 0) ? (now - data->handle) % data->period : 0;

            data->validate = true;
            data->timer->SetScheduling(true);
            data->handle = now + data->period - delay;

            _heap.Insert(data);
        }
        else
        {
            _idxMap.erase(data->timerId);
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

bool LLBC_TimerScheduler::IsDstroyed() const
{
    return _destroyed;
}

int LLBC_TimerScheduler::Schedule(LLBC_BaseTimer *timer)
{
    LLBC_TimerData *data = new LLBC_TimerData;
    data->handle = LLBC_GetMilliSeconds() + timer->GetDueTime();
    data->timerId = ++ _maxTimerId;
    data->dueTime = timer->GetDueTime();
    data->period = timer->GetPeriod();
    data->repeatTimes = 0;
    data->timer = timer;
    data->validate = true;

    timer->SetTimerId(_maxTimerId);
    timer->SetScheduling(true);

    _heap.Insert(data);
    _idxMap.insert(std::make_pair(_maxTimerId, data));

    return LLBC_OK;
}

int LLBC_TimerScheduler::Cancel(LLBC_BaseTimer *timer)
{
    _IdxMap::iterator iter = _idxMap.find(timer->GetTimerId());
    if (iter == _idxMap.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    LLBC_TimerData *data = iter->second;
    ASSERT(data->timer == timer && 
        "Timer manager internal error, LLBC_TimerData::timer != argument: timer!");

    _idxMap.erase(data->timerId);

    data->validate = false;
    data->timer->SetScheduling(false);
    data->timer->OnCancel();

    return LLBC_OK;
}

void LLBC_TimerScheduler::CancelAll()
{
    if (_destroyed)
        return;

    std::vector<LLBC_TimerId> timerIds;
    for (_IdxMap::iterator iter = _idxMap.begin();
         iter != _idxMap.end();
         iter++)
    {
        LLBC_TimerData *data = iter->second;
        if (data->validate)
            timerIds.push_back(data->timerId);
    }

    for (size_t i = 0; i < timerIds.size(); i++)
    {
        _IdxMap::iterator iter = _idxMap.find(timerIds[i]);
        if (iter != _idxMap.end())
        {
            LLBC_TimerData *data = iter->second;
            if (UNLIKELY(!data->validate))
                return;

            data->timer->Cancel();
        }
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
