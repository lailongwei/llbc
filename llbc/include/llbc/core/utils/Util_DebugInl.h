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
#include "llbc/core/os/OS_SysConf.h"
#include "llbc/core/os/OS_Process.h"
#include "llbc/core/file/Directory.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#include <Psapi.h>
#endif
__LLBC_NS_BEGIN

inline LLBC_Stopwatch::LLBC_Stopwatch(bool autoStart, bool traceMem)
: _beginTime(autoStart ? LLBC_RdTsc() : 0llu)
, _elapsedTime(0llu)

, _traceMemEnabled(traceMem)
, _beginMemSnapshot{0LL, 0LL, 0LL}
{
    if (_traceMemEnabled)
        GetMemSnapshot(_beginMemSnapshot);
}

inline LLBC_Stopwatch::LLBC_Stopwatch(uint64 elapsedTicks, bool continueMeasuring, bool traceMem)
: _beginTime(continueMeasuring ? LLBC_RdTsc() : 0llu)
, _elapsedTime(elapsedTicks)

, _traceMemEnabled(traceMem)
, _beginMemSnapshot{0LL, 0LL, 0LL}
{
    if (_traceMemEnabled)
        GetMemSnapshot(_beginMemSnapshot);
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

    if (_traceMemEnabled)
        GetMemSnapshot(_beginMemSnapshot);
}

inline void LLBC_Stopwatch::Restart()
{
    _elapsedTime = 0;
    _beginTime = LLBC_RdTsc();

    if (_traceMemEnabled)
        GetMemSnapshot(_beginMemSnapshot);
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

inline bool LLBC_Stopwatch::IsTraceMemEnabled() const
{
    return _traceMemEnabled;
}

inline LLBC_MemSnapshot LLBC_Stopwatch::GetMemSnapshotDiff() const
{
    if (!_traceMemEnabled)
        return {};

    LLBC_MemSnapshot endMemSnapshot{};
    if (!GetMemSnapshot(endMemSnapshot))
        return {};

    return {endMemSnapshot._virt - _beginMemSnapshot._virt,
            endMemSnapshot._res - _beginMemSnapshot._res,
            endMemSnapshot._shr - _beginMemSnapshot._shr};
}

inline LLBC_String LLBC_Stopwatch::ToString() const
{
    const uint64 nanos = ElapsedNanos();
    const auto memDiff = GetMemSnapshotDiff();

    LLBC_String repr;
    if (_traceMemEnabled)
        repr.format("%.03f ms, memDiff(virt:%lld res:%lld shr:%lld)", 
                    nanos / 1000000.0,
                    memDiff._virt, 
                    memDiff._res,
                    memDiff._shr);
    else
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

inline bool LLBC_Stopwatch::GetMemSnapshot(LLBC_MemSnapshot &snapshot)
{
#if LLBC_TARGET_PLATFORM_LINUX
    FILE *statmFile = fopen("/proc/self/statm", "r");
    if (LIKELY(statmFile)) 
    {
        sint64 memVirt{}, memRes{}, memShr{};
        const auto readCount = fscanf(statmFile, "%lld %lld %lld", &memVirt, &memRes, &memShr);
        fclose(statmFile);

        if (readCount == 3) 
        {
            snapshot._virt = memVirt * LLBC_pageSize;
            snapshot._res = memRes * LLBC_pageSize;
            snapshot._shr = memShr * LLBC_pageSize;
            return true;
        }
    }
#elif LLBC_TARGET_PLATFORM_WIN32 
    const HANDLE hProcess = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc))) 
    {
        snapshot._virt = static_cast<sint64>(pmc.PrivateUsage);
        snapshot._res = static_cast<sint64>(pmc.WorkingSetSize);
        snapshot._shr = static_cast<sint64>(pmc.WorkingSetSize - pmc.PrivateUsage);
        return true;
    }
#endif
    return false;
}

__LLBC_NS_END

