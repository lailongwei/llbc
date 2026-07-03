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

#include "llbc/core/thread/Guard.h"

#include "llbc/comm/ServiceLoadSampler.h"

__LLBC_NS_BEGIN

LLBC_ServiceRecentLoadInfo::LLBC_ServiceRecentLoadInfo()
: updateTimes(0)
, overloadTimes(0)
{
}

__LLBC_ServiceLoadSampler::__LLBC_ServiceLoadSampler()
: _loadSampleRing(0)
{
    memset(&_curLoadSample, 0, sizeof(_curLoadSample));
}

__LLBC_ServiceLoadSampler::~__LLBC_ServiceLoadSampler()
{
    Clear();
}

void __LLBC_ServiceLoadSampler::Init(int loadSampleCount)
{
    Clear();

    if (loadSampleCount <= 0)
        return;

    if (loadSampleCount > LLBC_CFG_COMM_MAX_SERVICE_LOAD_SAMPLE_COUNT)
        loadSampleCount = LLBC_CFG_COMM_MAX_SERVICE_LOAD_SAMPLE_COUNT;

    _loadSampleRing.ReCapacity(static_cast<size_t>(loadSampleCount));

    memset(&_curLoadSample, 0, sizeof(_curLoadSample));
}

void __LLBC_ServiceLoadSampler::Clear()
{
    if (IsEnabled())
    {
        LLBC_LockGuard guard(_loadSampleLock);
        _loadSampleRing.Clear();
    }

    memset(&_curLoadSample, 0, sizeof(_curLoadSample));
}

void __LLBC_ServiceLoadSampler::Collect(sint64 begMillis,
                                        sint64 endMillis,
                                        sint64 frameInterval)
{
    sint64 workingTime = endMillis - begMillis;
    if (UNLIKELY(workingTime < 0))
        workingTime = 0;

    const bool overloaded = (frameInterval > 0 && workingTime >= frameInterval);

    // Lazy-init current sample on first call after start.
    if (UNLIKELY(_curLoadSample.beginStatTimeInMillis == 0))
    {
        _curLoadSample.beginStatTimeInMillis = begMillis;
        _curLoadSample.lastStatTimeInMillis = endMillis;
        _curLoadSample.workingTime = workingTime;
        _curLoadSample.updateTimes = 1;
        _curLoadSample.overloadTimes = overloaded ? 1 : 0;
        return;
    }

    const sint64 sampleElapsed = endMillis - _curLoadSample.beginStatTimeInMillis;

    // Still in current sample window, just accumulate.
    if (LIKELY(sampleElapsed < LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL * 1000))
    {
        _curLoadSample.lastStatTimeInMillis = endMillis;
        _curLoadSample.workingTime += workingTime;
        _curLoadSample.updateTimes += 1;
        if (overloaded)
            _curLoadSample.overloadTimes += 1;
        return;
    }

    // Current sample window finished, accumulate this frame and push into ring buffer.
    _curLoadSample.lastStatTimeInMillis = endMillis;
    _curLoadSample.workingTime += workingTime;
    _curLoadSample.updateTimes += 1;
    if (overloaded)
        _curLoadSample.overloadTimes += 1;

    // Push completed sample(s) into ring buffer.
    LLBC_LockGuard guard(_loadSampleLock);
    if (LIKELY(IsEnabled()))
    {
        // Push the just-completed sample.
        if (UNLIKELY(_loadSampleRing.IsFull()))
            _loadSampleRing.Pop();
        _loadSampleRing.Push(_curLoadSample);

        // Fill empty samples for skipped intervals
        sint64 skippedIntervals =
            (sampleElapsed / (LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL * 1000)) - 1;
        if (skippedIntervals > 0)
        {
            const sint64 maxSkip = static_cast<sint64>(_loadSampleRing.GetCapacity());
            if (skippedIntervals > maxSkip)
                skippedIntervals = maxSkip;

            sint64 skipBeginTime = _curLoadSample.beginStatTimeInMillis +
                                   LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL * 1000;
            for (sint64 i = 0; i < skippedIntervals; ++i)
            {
                _ServiceLoadSample emptySample;
                emptySample.beginStatTimeInMillis = skipBeginTime;
                emptySample.lastStatTimeInMillis = skipBeginTime + LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL * 1000;
                emptySample.workingTime = 0;
                emptySample.updateTimes = 0;
                emptySample.overloadTimes = 0;

                if (UNLIKELY(_loadSampleRing.IsFull()))
                    _loadSampleRing.Pop();
                _loadSampleRing.Push(emptySample);

                skipBeginTime += LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL * 1000;
            }
        }
    }

    // Start a new sample window from <endMillis>.
    _curLoadSample.beginStatTimeInMillis = endMillis;
    _curLoadSample.lastStatTimeInMillis = endMillis;
    _curLoadSample.workingTime = 0;
    _curLoadSample.updateTimes = 0;
    _curLoadSample.overloadTimes = 0;
}

int __LLBC_ServiceLoadSampler::GetRecentLoadInfo(const LLBC_TimeSpan &recentTime,
                                                 LLBC_ServiceRecentLoadInfo &loadInfo) const
{
    // Reset output.
    loadInfo.recentTime = LLBC_TimeSpan::zero;
    loadInfo.workingTime = LLBC_TimeSpan::zero;
    loadInfo.updateTimes = 0;
    loadInfo.overloadTimes = 0;

    if (UNLIKELY(recentTime <= LLBC_TimeSpan::zero))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (UNLIKELY(!IsEnabled()))
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_ALLOW);
        return LLBC_FAILED;
    }

    const sint64 wantedMillis = recentTime.GetTotalMillis();

    // Prepare snapshot buffer: use stack buffer if small enough, otherwise heap buffer.
    const size_t maxSamples = _loadSampleRing.GetCapacity();
    _ServiceLoadSample stackBuf[64];
    _ServiceLoadSample *snapshot = stackBuf;
    if (maxSamples > sizeof(stackBuf) / sizeof(stackBuf[0]))
        snapshot = static_cast<_ServiceLoadSample *>(malloc(sizeof(_ServiceLoadSample) * maxSamples));
    LLBC_Defer(LLBC_DoIf(snapshot != stackBuf, free(snapshot)));

    // Snapshot ring buffer under lock.
    size_t snapshotCnt = 0;
    {
        LLBC_LockGuard guard(_loadSampleLock);

        // Drain into snapshot then push back.
        const size_t cnt = _loadSampleRing.GetSize();
        for (size_t i = 0; i < cnt; ++i)
            snapshot[i] = _loadSampleRing.Pop();
        for (size_t i = 0; i < cnt; ++i)
            _loadSampleRing.Push(snapshot[i]);

        snapshotCnt = cnt;
    }

    if (snapshotCnt == 0)
        return LLBC_OK;

    // Walk from newest to oldest, accumulate until covered time >= wantedMillis.
    sint64 accMillis = 0;
    sint64 accWorking = 0;
    size_t accUpdateTimes = 0;
    size_t accOverloadTimes = 0;
    for (size_t i = snapshotCnt; i > 0; --i)
    {
        const _ServiceLoadSample &s = snapshot[i - 1];
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
