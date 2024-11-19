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


#pragma once

#include "llbc/core/os/OS_Time.h"

__LLBC_NS_BEGIN

inline LLBC_Stopwatch::LLBC_Stopwatch(bool autoStart)
: _beginTime(autoStart ? LLBC_RdTsc() : 0llu)
, _elapsedTime(0llu)
{
}

inline void LLBC_Stopwatch::Resume()
{
    if (_beginTime == 0)
        _beginTime = LLBC_RdTsc();
}

inline void LLBC_Stopwatch::Pause()
{
    if (_beginTime != 0)
    {
        _elapsedTime += LLBC_RdTsc() - _beginTime;
        _beginTime = 0;
    }
}

inline void LLBC_Stopwatch::Reset()
{
    _beginTime = 0;
    _elapsedTime = 0;
}

inline LLBC_TimeSpan LLBC_Stopwatch::Elapsed() const
{
    #if LLBC_SUPPORT_RDTSC
    if (_beginTime != 0)
        return LLBC_TimeSpan(static_cast<sint64>(
            (LLBC_RdTsc() - _beginTime + _elapsedTime) * 1000000llu / _frequency));
    else
        return LLBC_TimeSpan(static_cast<sint64>(_elapsedTime * 1000000llu / _frequency));
    #else // Unsupported rdtsc
    if (_beginTime != 0)
        return LLBC_TimeSpan(static_cast<sint64>(LLBC_RdTsc() - _beginTime + _elapsedTime));
    else
        return LLBC_TimeSpan(static_cast<sint64>(_elapsedTime));
    #endif // LLBC_SUPPORT_RDTSC
}

inline uint64 LLBC_Stopwatch::ElapsedNanos() const
{
    #if LLBC_SUPPORT_RDTSC
    if (_beginTime != 0)
        return (LLBC_RdTsc() - _beginTime + _elapsedTime) * 1000000000llu / _frequency;
    else
        return _elapsedTime * 1000000000llu / _frequency;
    #else // Unsupported rdtsc
    if (_beginTime != 0)
        return (LLBC_RdTsc() - _beginTime + _elapsedTime) * LLBC_TimeConst::numOfNanosPerMicrosecond;
    else
        return _elapsedTime * LLBC_TimeConst::numOfNanosPerMicrosecond;
    #endif // LLBC_SUPPORT_RDTSC
}

inline uint64 LLBC_Stopwatch::ElapsedTicks() const
{
    if (_beginTime != 0)
        return LLBC_RdTsc() - _beginTime + _elapsedTime;
    else
        return _elapsedTime;
}

inline LLBC_String LLBC_Stopwatch::ToString() const
{
    const uint64 nanos = ElapsedNanos();

    LLBC_String repr;
    repr.format("%.03f ms", nanos / 1000000.0);

    return repr;
}

inline void LLBC_Stopwatch::InitFrequency()
{
#if LLBC_SUPPORT_RDTSC
    _frequency = LLBC_GetCpuCounterFrequency();
#else
    _frequency = LLBC_TimeConst::numOfMicrosPerSecond;
#endif
}

__LLBC_NS_END

