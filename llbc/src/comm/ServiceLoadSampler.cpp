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

#include "llbc/comm/ServiceLoadSampler.h"

__LLBC_NS_BEGIN

LLBC_ServiceRecentLoadInfo::LLBC_ServiceRecentLoadInfo()
: updateTimes(0)
, overloadTimes(0)
{
}

void LLBC_ServiceRecentLoadInfo::Reset()
{
    recentTime = LLBC_TimeSpan::zero;
    workingTime = LLBC_TimeSpan::zero;
    updateTimes = 0;
    overloadTimes = 0;
}

LLBC_String LLBC_ServiceRecentLoadInfo::ToString() const
{
    return LLBC_String().format(
        "ServiceRecentLoadInfo[recentTime:%s, workingTime:%s, "
        "updateTimes:%zu, overloadTimes:%zu]",
        recentTime.ToString().c_str(), workingTime.ToString().c_str(),
        updateTimes, overloadTimes);
}

__LLBC_ServiceLoadSampler::__LLBC_ServiceLoadSampler()
: _loadSampleRing(0)
{
}

__LLBC_ServiceLoadSampler::~__LLBC_ServiceLoadSampler()
{
    Clear();
}

void __LLBC_ServiceLoadSampler::Init(const LLBC_TimeSpan &loadSampleTime)
{
    if (loadSampleTime.GetTotalSeconds() <= 0)
        return;

    const int sampleSeconds = std::min(loadSampleTime.GetTotalSeconds(),
                                       LLBC_CFG_COMM_MAX_SERVICE_LOAD_SAMPLE_TIME);
    const size_t sampleCount = static_cast<size_t>(
        (sampleSeconds + LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL - 1) /
        LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL);

    LLBC_LockGuard guard(_loadSampleLock);
    _loadSampleRing.ReCapacity(sampleCount);
}

void __LLBC_ServiceLoadSampler::Clear()
{
    LLBC_LockGuard guard(_loadSampleLock);
    if (IsEnabled())
        _loadSampleRing.Clear();
}

void __LLBC_ServiceLoadSampler::Collect(sint64 begMillis,
                                        sint64 endMillis,
                                        sint64 frameInterval)
{
    LLBC_LockGuard guard(_loadSampleLock);
    
    if (UNLIKELY(!IsEnabled()))
        return;

    sint64 workingTime = endMillis - begMillis;
    if (UNLIKELY(workingTime < 0))
        workingTime = 0;

    const bool overloaded = (frameInterval > 0 && workingTime >= frameInterval);
    constexpr sint64 sampleIntervalMillis = LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL * 1000;

    // Lazy-init current sample on first call after start.
    if (UNLIKELY(_loadSampleRing.IsEmpty()))
    {
        _ServiceLoadSample newSample;
        newSample.beginStatTimeInMillis = begMillis;
        newSample.lastStatTimeInMillis = endMillis;
        newSample.workingTime = workingTime;
        newSample.updateTimes = 1;
        newSample.overloadTimes = overloaded ? 1 : 0;
        _loadSampleRing.Push(newSample);
        return;
    }

    _ServiceLoadSample &cur = _loadSampleRing.Tail();
    const sint64 sampleElapsed = endMillis - cur.beginStatTimeInMillis;

    // Accumulate this frame into current sample window.
    cur.lastStatTimeInMillis = endMillis;
    cur.workingTime += workingTime;
    cur.updateTimes += 1;
    if (overloaded)
        cur.overloadTimes += 1;

    // Still in current sample window.
    if (LIKELY(sampleElapsed < sampleIntervalMillis))
        return;

    // Current sample window finished, fill empty samples for skipped intervals.
    const sint64 curBegin = cur.beginStatTimeInMillis;
    sint64 skippedIntervals = (sampleElapsed / sampleIntervalMillis) - 1;
    if (skippedIntervals > 0)
    {
        const sint64 maxSkip = static_cast<sint64>(_loadSampleRing.GetCapacity());
        if (skippedIntervals > maxSkip)
            skippedIntervals = maxSkip;

        sint64 skipBeginTime = curBegin + sampleIntervalMillis;
        for (sint64 i = 0; i < skippedIntervals; ++i)
        {
            _ServiceLoadSample emptySample;
            emptySample.beginStatTimeInMillis = skipBeginTime;
            emptySample.lastStatTimeInMillis = skipBeginTime + sampleIntervalMillis;
            emptySample.workingTime = 0;
            emptySample.updateTimes = 0;
            emptySample.overloadTimes = 0;

            if (UNLIKELY(_loadSampleRing.IsFull()))
                _loadSampleRing.Pop();
            _loadSampleRing.Push(emptySample);

            skipBeginTime += sampleIntervalMillis;
        }
    }

    // Start a new sample window from <endMillis>.
    _ServiceLoadSample newSample;
    newSample.beginStatTimeInMillis = endMillis;
    newSample.lastStatTimeInMillis = endMillis;
    newSample.workingTime = 0;
    newSample.updateTimes = 0;
    newSample.overloadTimes = 0;
    if (UNLIKELY(_loadSampleRing.IsFull()))
        _loadSampleRing.Pop();
    _loadSampleRing.Push(newSample);
}

int __LLBC_ServiceLoadSampler::GetRecentLoadInfo(const LLBC_TimeSpan &recentTime,
                                                 LLBC_ServiceRecentLoadInfo &loadInfo) const
{
    LLBC_LockGuard guard(_loadSampleLock);

    // Reset output.
    loadInfo.Reset();

    if (UNLIKELY(!IsEnabled()))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    if (UNLIKELY(recentTime <= LLBC_TimeSpan::zero))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    const sint64 wantedMillis = recentTime.GetTotalMillis();

    // Walk from newest to oldest, accumulate until covered time >= wantedMillis.
    sint64 accMillis = 0;
    sint64 accWorking = 0;
    size_t accUpdateTimes = 0;
    size_t accOverloadTimes = 0;
    const size_t sampleCnt = _loadSampleRing.GetSize();
    for (size_t i = sampleCnt; i > 0; --i)
    {
        const _ServiceLoadSample &s = _loadSampleRing.GetElem(i - 1);
        const sint64 sampleSpan = s.lastStatTimeInMillis - s.beginStatTimeInMillis;
        accMillis += sampleSpan > 0 ? sampleSpan : 0;
        accWorking += s.workingTime;
        accUpdateTimes += s.updateTimes;
        accOverloadTimes += s.overloadTimes;

        if (accMillis >= wantedMillis)
            break;
    }

    loadInfo.recentTime = LLBC_TimeSpan::FromMillis(accMillis);
    loadInfo.workingTime = LLBC_TimeSpan::FromMillis(accWorking);
    loadInfo.updateTimes = accUpdateTimes;
    loadInfo.overloadTimes = accOverloadTimes;

    return LLBC_OK;
}

__LLBC_NS_END

std::ostream &operator<<(std::ostream &o, const LLBC_NS LLBC_ServiceRecentLoadInfo &loadInfo)
{
    return o <<loadInfo.ToString();
}
