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

#include "llbc/core/log/LoggerMgr.h"
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
, _beginMemSnap{0LL, 0LL, 0LL}
{
    if (_traceMemEnabled)
        GetMemSnapshot(_beginMemSnap);
}

inline LLBC_Stopwatch::LLBC_Stopwatch(uint64 elapsedTicks, bool continueMeasuring)
: _beginTime(continueMeasuring ? LLBC_RdTsc() : 0llu)
, _elapsedTime(elapsedTicks)

, _traceMemEnabled(false)
, _beginMemSnap{0LL, 0LL, 0LL}
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

    _beginMemSnap = {0LL, 0LL, 0LL};
}

inline void LLBC_Stopwatch::Restart()
{
    _elapsedTime = 0;
    _beginTime = LLBC_RdTsc();

    _beginMemSnap = {0LL, 0LL, 0LL};
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

    LLBC_MemSnapshot endMemSnap{};
    if(!GetMemSnapshot(endMemSnap))
        return {};

    return {endMemSnap._memVirt - _beginMemSnap._memVirt,
            endMemSnap._memRes - _beginMemSnap._memRes,
            endMemSnap._memShr - _beginMemSnap._memShr};
}

inline LLBC_String LLBC_Stopwatch::ToString() const
{
    const uint64 nanos = ElapsedNanos();
    const auto memDiff = GetMemSnapshotDiff();

    LLBC_String repr;
    if(_traceMemEnabled)
        repr.format("%.03f ms, memDiff(virt:%lld res:%lld shr:%lld)", 
                    nanos / 1000000.0,
                    memDiff._memVirt, 
                    memDiff._memRes,
                    memDiff._memShr);
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
            snapshot._memVirt = memVirt * LLBC_pageSize;
            snapshot._memRes = memRes * LLBC_pageSize;
            snapshot._memShr = memShr * LLBC_pageSize;
            return true;
        }
    }
#elif LLBC_TARGET_PLATFORM_WIN32 
    const HANDLE hProcess = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc))) 
    {
        snapshot._memVirt = static_cast<sint64>(pmc.PrivateUsage);
        snapshot._memRes = static_cast<sint64>(pmc.WorkingSetSize);
        snapshot._memShr = static_cast<sint64>(pmc.WorkingSetSize - pmc.PrivateUsage);
        return true;
    }
#endif
    return false;
}

inline LLBC_FuncTracer::LLBC_FuncTracer(const LLBC_CString &fileName, 
                                        int lineNo,
                                        const LLBC_CString &funcName, 
                                        bool traceMem,
                                        sint64 uniqueId,
                                        const LLBC_Logger *logger)
: _logger(nullptr)
, _stopWatch(true, traceMem)
{
    LLBC_String uniqueStr;
    uniqueStr.format("%lld", uniqueId);
    Init(fileName, lineNo, funcName, traceMem, uniqueStr);
}

inline LLBC_FuncTracer::LLBC_FuncTracer(const LLBC_CString &fileName, 
                                        int lineNo,
                                        const LLBC_CString &funcName, 
                                        bool traceMem,
                                        const LLBC_CString &uniqueStr,
                                        const LLBC_Logger *logger)
: _logger(logger)
, _stopWatch(true, traceMem)
{
    Init(fileName, lineNo, funcName, traceMem, uniqueStr);
}

inline void LLBC_FuncTracer::Init(const LLBC_CString &fileName, 
                                  int lineNo,
                                  const LLBC_CString &funcName, 
                                  bool traceMem,
                                  const LLBC_CString &uniqueStr)
{
    if(!_logger)
    {
        _logger = LLBC_LoggerMgrSingleton->GetRootLogger();
        if (UNLIKELY(!_logger))
            return;
    }

    if (_logger->GetLogLevel() > LLBC_LogLevel::Trace)
        return;

    _traceUniqInfo.format("%s:%d:%s:%s",
                          LLBC_Directory::BaseName(fileName).c_str(),
                          lineNo,
                          funcName.c_str(),
                          uniqueStr.c_str());

    LLOG_TRACE3("FuncTrace", "%s|Enter(trace mem?:%s)", _traceUniqInfo.c_str(), traceMem ? "true" : "false");
}

inline LLBC_FuncTracer::~LLBC_FuncTracer()
{
    if (LIKELY(_logger) && _logger->GetLogLevel() >= LLBC_LogLevel::Trace) 
    {
        const auto memDiff = _stopWatch.GetMemSnapshotDiff();
        const auto cost = _stopWatch.Elapsed();
        LLOG_TRACE3("FuncTrace",
                    "%s|Leave(trace mem?:%s) cost:%lld.%03lld ms, memory diff(virt:%lld res:%lld share:%lld)",
                    _traceUniqInfo.c_str(),
                    _stopWatch.IsTraceMemEnabled() ? "true" : "false",
                    cost.GetTotalMillis(),
                    cost.GetTotalMicros() % 1000,
                    memDiff._memVirt,
                    memDiff._memRes,
                    memDiff._memShr);
    }
}
__LLBC_NS_END

