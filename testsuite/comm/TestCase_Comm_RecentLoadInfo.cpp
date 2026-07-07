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

#include "comm/TestCase_Comm_RecentLoadInfo.h"

#include <sstream>

namespace
{
    constexpr int kSampleIntervalSec = LLBC_CFG_COMM_SERVICE_LOAD_SAMPLE_INTERVAL;
    constexpr int kSampleIntervalMillis = kSampleIntervalSec * 1000;

    class IdleComp final : public LLBC_Component
    {
    public:
        void OnUpdate() override {}
    };

    // Sleep in OnUpdate() to make working time exceed frame interval.
    class OverloadComp final : public LLBC_Component
    {
    public:
        explicit OverloadComp(int sleepMillis) : _sleepMillis(sleepMillis) {}
        void OnUpdate() override
        {
            LLBC_Sleep(_sleepMillis);
        }

    private:
        int _sleepMillis;
    };

    class QueryTask final : public LLBC_Task
    {
    public:
        QueryTask(LLBC_Service *svc, int durationMillis)
        : _svc(svc)
        , _duration(durationMillis)
        , _queryTimes(0)
        , _failedTimes(0)
        {}

        void Svc() override
        {
            const sint64 endAt = LLBC_GetMilliseconds() + _duration;
            while (LLBC_GetMilliseconds() < endAt)
            {
                LLBC_ServiceRecentLoadInfo info;
                const int ret = _svc->GetRecentLoadInfo(LLBC_TimeSpan::FromSeconds(60), info);
                ++_queryTimes;
                if (ret != LLBC_OK)
                    ++_failedTimes;

                LLBC_Sleep(1);
            }
        }

        void Cleanup() override {}

        int GetQueryTimes() const { return _queryTimes; }
        int GetFailedTimes() const { return _failedTimes; }

    private:
        LLBC_Service *_svc;
        int _duration;
        int _queryTimes;
        int _failedTimes;
    };

    void DumpLoadInfo(const char *tag, const LLBC_ServiceRecentLoadInfo &info)
    {
        const sint64 recentMs = info.recentTime.GetTotalMillis();
        const sint64 workMs = info.workingTime.GetTotalMillis();
        const double loadPercent = recentMs > 0 ?
            (static_cast<double>(workMs) * 100.0 / static_cast<double>(recentMs)) : 0.0;
        const double recentSec = static_cast<double>(recentMs) / 1000.0;
        const double avgFps = recentSec > 0.0 ?
            (static_cast<double>(info.updateTimes) / recentSec) : 0.0;
        LLBC_PrintLn("[%s] recentTime:%lldms, workingTime:%lldms, load:%.2f%%, "
                     "updateTimes:%zu, avgFps:%.2f, overloadTimes:%zu",
                     tag, recentMs, workMs, loadPercent,
                     info.updateTimes, avgFps, info.overloadTimes);
    }
}

TestCase_Comm_RecentLoadInfo::TestCase_Comm_RecentLoadInfo()
{
}

TestCase_Comm_RecentLoadInfo::~TestCase_Comm_RecentLoadInfo()
{
}

int TestCase_Comm_RecentLoadInfo::Run(int argc, char *argv[])
{
    LLBC_PrintLn("==== Service RecentLoadInfo TestCase ====");
    LLBC_PrintLn("Sample interval(from Config.h): %d seconds", kSampleIntervalSec);

    struct SubTest
    {
        const char *name;
        int (TestCase_Comm_RecentLoadInfo::*func)();
    };
    const SubTest tests[] = {
        { "Disabled",                &TestCase_Comm_RecentLoadInfo::Test_Disabled                },
        { "InvalidArgs",             &TestCase_Comm_RecentLoadInfo::Test_InvalidArgs             },
        { "BasicStats",              &TestCase_Comm_RecentLoadInfo::Test_BasicStats              },
        { "OverloadDetect",          &TestCase_Comm_RecentLoadInfo::Test_OverloadDetect          },
        { "SlidingWindow",           &TestCase_Comm_RecentLoadInfo::Test_SlidingWindow           },
        { "ConcurrentQuery",         &TestCase_Comm_RecentLoadInfo::Test_ConcurrentQuery         },
        { "LargeSampleCount",        &TestCase_Comm_RecentLoadInfo::Test_LargeSampleCount        },
        { "ExceedMaxSampleCount",    &TestCase_Comm_RecentLoadInfo::Test_ExceedMaxSampleCount    },
        { "PartialSampleAtBoundary", &TestCase_Comm_RecentLoadInfo::Test_PartialSampleAtBoundary },
        { "ToString",                &TestCase_Comm_RecentLoadInfo::Test_ToString                },
    };

    int failedCnt = 0;
    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i)
    {
        LLBC_PrintLn("\n---- [Sub %zu] %s ----", i + 1, tests[i].name);
        if ((this->*(tests[i].func))() != LLBC_OK)
        {
            LLBC_PrintLn("[FAIL] %s, last error: %s", tests[i].name, LLBC_FormatLastError());
            ++failedCnt;
        }
        else
        {
            LLBC_PrintLn("[OK] %s", tests[i].name);
        }
    }

    LLBC_PrintLn("\n==== Done. total:%zu, failed:%d ====",
                 sizeof(tests) / sizeof(tests[0]), failedCnt);
    return failedCnt == 0 ? LLBC_OK : LLBC_FAILED;
}

// Sampling disabled(loadSampleTime=0): GetRecentLoadInfo() should return NOT_ALLOW.
int TestCase_Comm_RecentLoadInfo::Test_Disabled()
{
    LLBC_PrintLn("Start service with loadSampleTime=0(disabled), expect GetRecentLoadInfo() return NOT_ALLOW.");

    LLBC_Service *svc = LLBC_Service::Create("RLI_Disabled");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(50);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::zero;
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    LLBC_ServiceRecentLoadInfo info;
    const int ret = svc->GetRecentLoadInfo(LLBC_TimeSpan::FromSeconds(10), info);
    const int err = LLBC_GetLastError();

    if (ret == LLBC_OK)
    {
        LLBC_PrintLn("Expect GetRecentLoadInfo() failed when sampling disabled, but got OK.");
        return LLBC_FAILED;
    }
    if (err != LLBC_ERROR_NOT_ALLOW)
    {
        LLBC_PrintLn("Expect errno=NOT_ALLOW, but got %s.", LLBC_StrError(err));
        return LLBC_FAILED;
    }
    // Output must be zero-cleared on failure.
    if (info.recentTime != LLBC_TimeSpan::zero ||
        info.workingTime != LLBC_TimeSpan::zero ||
        info.updateTimes != 0 || info.overloadTimes != 0)
    {
        LLBC_PrintLn("Output info should be zero-cleared on failure.");
        return LLBC_FAILED;
    }

    LLBC_PrintLn("GetRecentLoadInfo returned ret:%d, err:%s, info zero-cleared. As expected.",
                 ret, LLBC_StrError(err));
    return LLBC_OK;
}

// Invalid recentTime(zero/negative) should be rejected with ARG error.
int TestCase_Comm_RecentLoadInfo::Test_InvalidArgs()
{
    LLBC_Service *svc = LLBC_Service::Create("RLI_InvalidArgs");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(50);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(4 * kSampleIntervalSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    // Both zero and negative recentTime must be rejected with ARG.
    LLBC_ServiceRecentLoadInfo info;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::zero, info) == LLBC_OK ||
        LLBC_GetLastError() != LLBC_ERROR_ARG)
    {
        LLBC_PrintLn("Expect ARG error on zero recentTime, last error: %s.",
                     LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromMillis(-100), info) == LLBC_OK ||
        LLBC_GetLastError() != LLBC_ERROR_ARG)
    {
        LLBC_PrintLn("Expect ARG error on negative recentTime, last error: %s.",
                     LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

// Idle service basic stats: recentTime/updateTimes>0, workingTime<=recentTime, avgFps around target.
int TestCase_Comm_RecentLoadInfo::Test_BasicStats()
{
    const int targetFps = 50;
    LLBC_Service *svc = LLBC_Service::Create("RLI_BasicStats");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(targetFps);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(8 * kSampleIntervalSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    const int waitSec = kSampleIntervalSec * 2 + 1;
    LLBC_PrintLn("Wait %ds for sampling...", waitSec);
    LLBC_Sleep(waitSec * 1000);

    LLBC_ServiceRecentLoadInfo info;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromSeconds(kSampleIntervalSec * 3), info) != LLBC_OK)
    {
        LLBC_PrintLn("GetRecentLoadInfo failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    DumpLoadInfo("BasicStats", info);

    if (info.recentTime <= LLBC_TimeSpan::zero)
    {
        LLBC_PrintLn("recentTime should be > 0.");
        return LLBC_FAILED;
    }
    if (info.updateTimes == 0)
    {
        LLBC_PrintLn("updateTimes should be > 0.");
        return LLBC_FAILED;
    }
    if (info.workingTime > info.recentTime)
    {
        LLBC_PrintLn("workingTime(%lldms) should not exceed recentTime(%lldms).",
                     info.workingTime.GetTotalMillis(),
                     info.recentTime.GetTotalMillis());
        return LLBC_FAILED;
    }
    if (info.overloadTimes > info.updateTimes / 2)
    {
        LLBC_PrintLn("Idle service should not be heavily overloaded(updateTimes:%zu, overloadTimes:%zu).",
                     info.updateTimes, info.overloadTimes);
        return LLBC_FAILED;
    }

    const double recentSec = info.recentTime.GetTotalMillis() / 1000.0;
    const double avgFps = info.updateTimes / recentSec;
    if (avgFps < targetFps * 0.5 || avgFps > targetFps * 1.5)
    {
        LLBC_PrintLn("AvgFps(%.2f) deviated too much from target(%d).", avgFps, targetFps);
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

// Force every frame overloaded, expect overloadTimes covers almost all frames.
int TestCase_Comm_RecentLoadInfo::Test_OverloadDetect()
{
    // FrameInterval=50ms, OnUpdate sleeps 80ms => every frame is overload.
    const int targetFps = 20;
    const int sleepPerFrame = 80;
    LLBC_Service *svc = LLBC_Service::Create("RLI_Overload");
    svc->AddComponent(new OverloadComp(sleepPerFrame));
    svc->SetFPS(targetFps);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(8 * kSampleIntervalSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    const int waitSec = kSampleIntervalSec * 2 + 1;
    LLBC_PrintLn("Wait %ds for overload sampling...", waitSec);
    LLBC_Sleep(waitSec * 1000);

    LLBC_ServiceRecentLoadInfo info;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromSeconds(kSampleIntervalSec * 3), info) != LLBC_OK)
    {
        LLBC_PrintLn("GetRecentLoadInfo failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    DumpLoadInfo("Overload", info);

    if (info.updateTimes == 0)
    {
        LLBC_PrintLn("updateTimes should be > 0.");
        return LLBC_FAILED;
    }
    // Expect >= 90% frames overloaded.
    if (info.overloadTimes < info.updateTimes * 9 / 10)
    {
        LLBC_PrintLn("Expect almost all frames overloaded, got overload:%zu/%zu.",
                     info.overloadTimes, info.updateTimes);
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

// Fill & evict ring, query with small vs. big recentTime, verify sliding window scales.
int TestCase_Comm_RecentLoadInfo::Test_SlidingWindow()
{
    // Run long enough to fill & evict the ring, then query with various recentTime.
    const int sampleCount = 3;
    const int waitSec = kSampleIntervalSec * (sampleCount + 2);
    LLBC_Service *svc = LLBC_Service::Create("RLI_SlidingWindow");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(50);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(sampleCount * kSampleIntervalSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    LLBC_PrintLn("Wait %ds(>= %d samples) to fill & evict ring buffer...",
                 waitSec, sampleCount);
    LLBC_Sleep(waitSec * 1000);

    // Query 1: minimal recentTime, expect only the newest sample.
    LLBC_ServiceRecentLoadInfo small;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromMillis(1), small) != LLBC_OK)
    {
        LLBC_PrintLn("Small query failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    DumpLoadInfo("SmallWindow", small);

    // Query 2: recentTime > ring capacity, expect scanning the whole ring.
    const sint64 ringCapMs =
        static_cast<sint64>(sampleCount) * kSampleIntervalMillis;
    LLBC_ServiceRecentLoadInfo big;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromMillis(ringCapMs * 2), big) != LLBC_OK)
    {
        LLBC_PrintLn("Big query failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    DumpLoadInfo("BigWindow", big);

    if (big.recentTime <= small.recentTime)
    {
        LLBC_PrintLn("Expect big.recentTime > small.recentTime.");
        return LLBC_FAILED;
    }
    if (big.updateTimes <= small.updateTimes)
    {
        LLBC_PrintLn("Expect big.updateTimes > small.updateTimes.");
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

// Multi-thread concurrent GetRecentLoadInfo(), expect all queries succeed(thread-safe).
int TestCase_Comm_RecentLoadInfo::Test_ConcurrentQuery()
{
    // Verify GetRecentLoadInfo() is thread-safe under concurrent queries.
    LLBC_Service *svc = LLBC_Service::Create("RLI_Concurrent");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(100);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(8 * kSampleIntervalSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    // Wait at least 1 sample interval so ring buffer has data.
    LLBC_Sleep(kSampleIntervalMillis + 500);
    constexpr int kThreadCnt = 4;
    constexpr int kThreadDurationMs = 3000;
    QueryTask *tasks[kThreadCnt] = { nullptr };
    for (int i = 0; i < kThreadCnt; ++i)
    {
        tasks[i] = new QueryTask(svc, kThreadDurationMs);
        if (tasks[i]->Activate(1) != LLBC_OK)
        {
            LLBC_PrintLn("Activate query task[%d] failed: %s", i, LLBC_FormatLastError());
            for (int j = 0; j <= i; ++j)
                delete tasks[j];
            return LLBC_FAILED;
        }
    }

    int totalQuery = 0;
    int totalFailed = 0;
    for (int i = 0; i < kThreadCnt; ++i)
    {
        tasks[i]->Wait();
        totalQuery += tasks[i]->GetQueryTimes();
        totalFailed += tasks[i]->GetFailedTimes();
        delete tasks[i];
    }

    LLBC_PrintLn("Total concurrent queries:%d, failed:%d.", totalQuery, totalFailed);

    if (totalQuery == 0)
    {
        LLBC_PrintLn("Concurrent queries should run > 0 times.");
        return LLBC_FAILED;
    }
    if (totalFailed != 0)
    {
        LLBC_PrintLn("Concurrent queries should all succeed, but %d failed.", totalFailed);
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

// Sample count > stackBuf(64) triggers heap-allocation path in GetRecentLoadInfo().
int TestCase_Comm_RecentLoadInfo::Test_LargeSampleCount()
{
    // stackBuf capacity is 64, use 100 to force the heap-allocation path.
    constexpr int kSampleCount = 100;

    LLBC_Service *svc = LLBC_Service::Create("RLI_LargeSampleCount");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(50);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(kSampleCount * kSampleIntervalSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    const int waitSec = kSampleIntervalSec * 2 + 1;
    LLBC_PrintLn("Wait %ds for sampling(sampleCount=%d, heap path)...",
                 waitSec, kSampleCount);
    LLBC_Sleep(waitSec * 1000);

    const int queryWindowSec = kSampleCount * kSampleIntervalSec;
    LLBC_ServiceRecentLoadInfo info;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromSeconds(queryWindowSec), info) != LLBC_OK)
    {
        LLBC_PrintLn("GetRecentLoadInfo failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    DumpLoadInfo("LargeSampleCount", info);

    if (info.recentTime <= LLBC_TimeSpan::zero || info.updateTimes == 0)
    {
        LLBC_PrintLn("Expect non-empty stats via heap path, got recent:%lldms, updates:%zu.",
                     info.recentTime.GetTotalMillis(), info.updateTimes);
        return LLBC_FAILED;
    }
    if (info.workingTime > info.recentTime)
    {
        LLBC_PrintLn("workingTime(%lldms) should not exceed recentTime(%lldms).",
                     info.workingTime.GetTotalMillis(),
                     info.recentTime.GetTotalMillis());
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

// sampleTime over config upper bound should be silently clamped, sampling still works.
int TestCase_Comm_RecentLoadInfo::Test_ExceedMaxSampleCount()
{
    // sampleTime over the config upper bound should be silently clamped.
    constexpr int kSampleTimeSec =
        LLBC_CFG_COMM_MAX_SERVICE_LOAD_SAMPLE_TIME + 1000;

    LLBC_Service *svc = LLBC_Service::Create("RLI_ExceedMaxSampleCount");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(50);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(kSampleTimeSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        LLBC_PrintLn("Start with sampleTimeSec=%d failed: %s",
                     kSampleTimeSec, LLBC_FormatLastError());
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    const int waitSec = kSampleIntervalSec + 1;
    LLBC_PrintLn("Wait %ds for sampling(requested sampleTimeSec=%d, expected clamped to %ds)...",
                 waitSec, kSampleTimeSec, LLBC_CFG_COMM_MAX_SERVICE_LOAD_SAMPLE_TIME);
    LLBC_Sleep(waitSec * 1000);

    const int queryWindowSec =
        LLBC_CFG_COMM_MAX_SERVICE_LOAD_SAMPLE_TIME;
    LLBC_ServiceRecentLoadInfo info;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromSeconds(queryWindowSec), info) != LLBC_OK)
    {
        LLBC_PrintLn("GetRecentLoadInfo failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    DumpLoadInfo("ExceedMaxSampleCount", info);

    if (info.updateTimes == 0)
    {
        LLBC_PrintLn("Expect updateTimes > 0 after clamping, got 0.");
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

// Query window crosses partial-full sample boundary, verify tail partial sample handling.
int TestCase_Comm_RecentLoadInfo::Test_PartialSampleAtBoundary()
{
    // Query window crosses partial-full sample boundary.
    const int sampleTimeSec = kSampleIntervalSec * 10;
    LLBC_Service *svc = LLBC_Service::Create("RLI_PartialBoundary");
    svc->AddComponent(new IdleComp);
    svc->SetFPS(50);
    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 1;
    startArgs.loadSampleTime = LLBC_TimeSpan::FromSeconds(sampleTimeSec);
    if (svc->Start(startArgs) != LLBC_OK)
    {
        delete svc;
        return LLBC_FAILED;
    }
    LLBC_Defer(delete svc);

    // Wait 2.5 intervals so tail sample is partial.
    const int waitMs = kSampleIntervalMillis * 2 + kSampleIntervalMillis / 2;
    LLBC_PrintLn("Wait %dms so tail sample is partial...", waitMs);
    LLBC_Sleep(waitMs);

    // Request ~1.33 interval, crossing the partial-full boundary.
    const sint64 requestedMs = kSampleIntervalMillis + kSampleIntervalMillis / 3;
    LLBC_ServiceRecentLoadInfo info;
    if (svc->GetRecentLoadInfo(LLBC_TimeSpan::FromMillis(requestedMs), info) != LLBC_OK)
    {
        LLBC_PrintLn("GetRecentLoadInfo failed: %s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }
    DumpLoadInfo("PartialBoundary", info);

    const sint64 actualMs = info.recentTime.GetTotalMillis();
    if (actualMs < requestedMs)
    {
        LLBC_PrintLn("Expect recentTime(%lldms) >= requested(%lldms).",
                     actualMs, requestedMs);
        return LLBC_FAILED;
    }

    const sint64 maxAllowedMs = requestedMs + kSampleIntervalMillis;
    if (actualMs > maxAllowedMs)
    {
        LLBC_PrintLn("Expect recentTime(%lldms) <= requested + interval(%lldms).",
                     actualMs, maxAllowedMs);
        return LLBC_FAILED;
    }

    if (info.updateTimes == 0)
    {
        LLBC_PrintLn("Expect updateTimes > 0.");
        return LLBC_FAILED;
    }
    return LLBC_OK;
}

// Print LLBC_ServiceRecentLoadInfo::ToString() / operator<< output for visual check.
int TestCase_Comm_RecentLoadInfo::Test_ToString()
{
    LLBC_ServiceRecentLoadInfo zeroInfo;
    LLBC_PrintLn("Zero info ToString: %s", zeroInfo.ToString().c_str());

    LLBC_ServiceRecentLoadInfo info;
    info.recentTime = LLBC_TimeSpan::FromMillis(12345);
    info.workingTime = LLBC_TimeSpan::FromMillis(6789);
    info.updateTimes = 100;
    info.overloadTimes = 7;
    LLBC_PrintLn("Filled info ToString: %s", info.ToString().c_str());

    std::ostringstream oss;
    oss << info;
    LLBC_PrintLn("Filled info operator<<: %s", oss.str().c_str());

    return LLBC_OK;
}
