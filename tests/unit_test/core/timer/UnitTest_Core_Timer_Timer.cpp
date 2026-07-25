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

#include <llbc.h>
using namespace llbc;

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/timer/Timer.cpp
// @coverage-target: llbc/include/llbc/core/timer/TimerInl.h
// @coverage-target: llbc/src/core/timer/TimerScheduler.cpp

namespace
{

bool DriveUntil(LLBC_TimerScheduler &scheduler, const std::atomic<int> &count, int expected)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (count.load(std::memory_order_acquire) < expected)
    {
        scheduler.Update();
        if (std::chrono::steady_clock::now() >= deadline)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return true;
}

struct TimerMemberCallbackReceiver
{
    void OnTimeout(LLBC_Timer *)
    {
        ++timeoutCount;
    }

    void OnCancel(LLBC_Timer *)
    {
        ++cancelCount;
    }

    int timeoutCount = 0;
    int cancelCount = 0;
};

class SelfDeletingTimer final : public LLBC_Timer
{
public:
    SelfDeletingTimer(LLBC_TimerScheduler *scheduler, std::atomic<int> *cancelCount)
    : LLBC_Timer(nullptr, nullptr, scheduler)
    , _cancelCount(cancelCount)
    {
    }

    void OnCancel() override
    {
        _cancelCount->fetch_add(1, std::memory_order_release);
        delete this;
    }

private:
    std::atomic<int> *_cancelCount;
};

} // namespace

// Timers constructed on the entry thread resolve its default scheduler lazily.
// This also verifies unscheduled metadata and the normalization of past absolute
// times, zero periods, and a zero trigger count.
TEST(TimerTest, ExposesDefaultStateAndUsesEntryThreadScheduler)
{
    LLBC_Timer timer;
    const LLBC_Timer &constTimer = timer;

    EXPECT_EQ(timer.GetTimerId(),
              static_cast<decltype(timer.GetTimerId())>(LLBC_INVALID_TIMER_ID));
    EXPECT_EQ(timer.GetFirstPeriod(), LLBC_TimeSpan::zero);
    EXPECT_EQ(timer.GetPeriod(), LLBC_TimeSpan::zero);
    EXPECT_EQ(timer.GetTotalTriggerCount(), static_cast<size_t>(LLBC_INFINITE));
    EXPECT_EQ(timer.GetTriggeredCount(), 0lu);
    EXPECT_EQ(constTimer.GetTimerData(), LLBC_Variant::nil);
    EXPECT_EQ(timer.GetTimeoutTime(), LLBC_Time::utcBegin);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_FALSE(timer.IsHandlingTimeout());
    EXPECT_FALSE(timer.IsHandlingCancel());
    EXPECT_FALSE(static_cast<bool>(timer.GetTimeoutHandler()));
    EXPECT_FALSE(static_cast<bool>(timer.GetCancelHandler()));
    EXPECT_NE(timer.ToString().find("trigger:0/-1"), static_cast<LLBC_String::size_type>(-1));

    timer.GetTimerData() = 42;
    EXPECT_EQ(constTimer.GetTimerData(), 42);

    timer.OnTimeout();
    timer.OnCancel();

    LLBC_TimerScheduler *scheduler = LLBC_TimerScheduler::GetCurrentThreadScheduler();
    ASSERT_NE(scheduler, nullptr);

    int timeoutCount = 0;
    LLBC_Time timeoutTime = LLBC_Time::utcBegin;
    timer.SetTimeoutHandler([&](LLBC_Timer *fired) {
        EXPECT_TRUE(fired->IsScheduled());
        EXPECT_TRUE(fired->IsHandlingTimeout());
        EXPECT_NE(fired->ToString().find("handlingTimeout:true"), static_cast<LLBC_String::size_type>(-1));
        timeoutTime = fired->GetTimeoutTime();
        ++timeoutCount;
    });

    ASSERT_EQ(timer.Schedule(LLBC_Time::Now() - LLBC_TimeSpan::oneMillisec,
                             LLBC_TimeSpan::zero,
                             0),
              LLBC_OK);
    EXPECT_EQ(timer.GetFirstPeriod(), LLBC_TimeSpan::zero);
    EXPECT_EQ(timer.GetPeriod(), LLBC_TimeSpan::zero);
    EXPECT_EQ(timer.GetTotalTriggerCount(), 1lu);

    scheduler->Update();
    EXPECT_EQ(timeoutCount, 1);
    EXPECT_NE(timeoutTime, LLBC_Time::utcBegin);
    EXPECT_EQ(timer.GetTriggeredCount(), 1lu);
    EXPECT_FALSE(timer.IsScheduled());
}

// Only the entry thread owns an implicit scheduler. Worker threads must supply
// a scheduler explicitly instead of silently creating a timer registration with
// no update loop.
TEST(TimerTest, RejectsImplicitSchedulingOutsideEntryThread)
{
    std::atomic<int> scheduleRet {LLBC_OK};
    std::atomic<int> scheduleError {LLBC_ERROR_SUCCESS};
    std::thread worker([&] {
        LLBC_Timer timer;
        scheduleRet.store(timer.Schedule(LLBC_TimeSpan::zero), std::memory_order_release);
        scheduleError.store(LLBC_GetLastError(), std::memory_order_release);
    });
    worker.join();

    EXPECT_EQ(scheduleRet.load(std::memory_order_acquire), LLBC_FAILED);
    EXPECT_EQ(scheduleError.load(std::memory_order_acquire), LLBC_ERROR_INVALID);
}

// The member-function overloads attach ordinary object callbacks, and
// scheduling clamps negative periods to zero before a one-shot dispatch.
TEST(TimerTest, BindsMemberCallbacksAndNormalizesScheduleArguments)
{
    LLBC_TimerScheduler scheduler;
    TimerMemberCallbackReceiver receiver;
    LLBC_Timer timer(nullptr, nullptr, &scheduler);
    timer.SetCancelHandler(nullptr);
    timer.SetTimeoutHandler(&receiver, &TimerMemberCallbackReceiver::OnTimeout);
    timer.SetCancelHandler(&receiver, &TimerMemberCallbackReceiver::OnCancel);
    timer.SetCancelHandler(&receiver, &TimerMemberCallbackReceiver::OnCancel);

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::FromMillis(-10),
                             LLBC_TimeSpan::FromMillis(-5),
                             0),
              LLBC_OK);
    EXPECT_EQ(timer.GetFirstPeriod(), LLBC_TimeSpan::zero);
    EXPECT_EQ(timer.GetPeriod(), LLBC_TimeSpan::zero);
    EXPECT_EQ(timer.GetTotalTriggerCount(), 1lu);

    scheduler.Update();
    EXPECT_EQ(receiver.timeoutCount, 1);
    EXPECT_EQ(receiver.cancelCount, 0);
    EXPECT_FALSE(timer.IsScheduled());

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::FromMillis(10)), LLBC_OK);
    ASSERT_EQ(timer.Cancel(), LLBC_OK);
    EXPECT_EQ(receiver.cancelCount, 1);

    timer.SetTimeoutHandler(nullptr);
    timer.SetCancelHandler(nullptr);
    timer.OnCancel();
    EXPECT_FALSE(static_cast<bool>(timer.GetTimeoutHandler()));
    EXPECT_FALSE(static_cast<bool>(timer.GetCancelHandler()));
}

// A one-shot timer records scheduler metadata, invokes its callback exactly once,
// and releases its scheduled state after the configured trigger count is reached.
TEST(TimerTest, SchedulesOneShotAndExposesMetadata)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> timeoutCount {0};
    LLBC_Timer timer(
        [&](LLBC_Timer *fired) {
            EXPECT_TRUE(fired->IsHandlingTimeout());
            timeoutCount.fetch_add(1, std::memory_order_release);
        },
        nullptr,
        &scheduler);

    timer.GetTimerData()["tag"] = LLBC_String("one-shot");
    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::zero, LLBC_TimeSpan::zero, 1), LLBC_OK);
    EXPECT_TRUE(timer.IsScheduled());
    EXPECT_NE(timer.GetTimerId(),
              static_cast<decltype(timer.GetTimerId())>(LLBC_INVALID_TIMER_ID));
    EXPECT_EQ(timer.GetTotalTriggerCount(), 1lu);
    EXPECT_EQ(timer.GetTriggeredCount(), 0lu);
    EXPECT_EQ(timer.GetTimerData()["tag"].As<LLBC_String>(), "one-shot");
    EXPECT_NE(timer.ToString().find("scheduled:true"), static_cast<LLBC_String::size_type>(-1));

    scheduler.Update();
    EXPECT_EQ(timeoutCount.load(std::memory_order_acquire), 1);
    EXPECT_EQ(timer.GetTriggeredCount(), 1lu);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);
}

// Repeating timers reschedule themselves until the configured count, while a
// disabled scheduler defers expiration without dropping the timer.
TEST(TimerTest, RepeatsAndHonorsSchedulerEnabledFlag)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> timeoutCount {0};
    LLBC_Timer timer(
        [&](LLBC_Timer *) { timeoutCount.fetch_add(1, std::memory_order_release); },
        nullptr,
        &scheduler);

    scheduler.SetEnabled(false);
    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::zero, LLBC_TimeSpan::FromMillis(1), 3), LLBC_OK);
    scheduler.Update();
    EXPECT_EQ(timeoutCount.load(std::memory_order_acquire), 0);
    EXPECT_TRUE(timer.IsScheduled());

    scheduler.SetEnabled(true);
    ASSERT_TRUE(DriveUntil(scheduler, timeoutCount, 3));
    scheduler.Update();
    EXPECT_EQ(timeoutCount.load(std::memory_order_acquire), 3);
    EXPECT_EQ(timer.GetTriggeredCount(), 3lu);
    EXPECT_FALSE(timer.IsScheduled());
}

// Cancellation invokes the cancel delegate once, preserves timer data for
// inspection, and is idempotent after a timer is no longer scheduled.
TEST(TimerTest, CancelsScheduledTimerAndInvokesCancelHandler)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> cancelCount {0};
    LLBC_Timer timer(
        nullptr,
        [&](LLBC_Timer *cancelled) {
            EXPECT_TRUE(cancelled->IsHandlingCancel());
            cancelCount.fetch_add(1, std::memory_order_release);
        },
        &scheduler);

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::FromMillis(100), LLBC_TimeSpan::FromMillis(100)), LLBC_OK);
    EXPECT_TRUE(timer.IsScheduled());
    EXPECT_EQ(timer.Cancel(), LLBC_OK);
    EXPECT_EQ(cancelCount.load(std::memory_order_acquire), 1);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_FALSE(timer.IsHandlingCancel());
    EXPECT_EQ(timer.Cancel(), LLBC_OK);

    timer.SetTimeoutHandler(nullptr);
    timer.SetCancelHandler(nullptr);
    EXPECT_FALSE(static_cast<bool>(timer.GetTimeoutHandler()));
    EXPECT_FALSE(static_cast<bool>(timer.GetCancelHandler()));
}

// Canceling from a timeout handler must run the cancel handler synchronously.
// Both handling-state flags are visible to the cancel callback, and the infinite
// timer is not rescheduled after the callback returns.
TEST(TimerTest, CancelsFromTimeoutAndReportsNestedHandlingState)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> timeoutCount {0};
    std::atomic<int> cancelCount {0};
    LLBC_Timer timer(
        [&](LLBC_Timer *fired) {
            EXPECT_TRUE(fired->IsScheduled());
            EXPECT_TRUE(fired->IsHandlingTimeout());
            EXPECT_FALSE(fired->IsHandlingCancel());
            timeoutCount.fetch_add(1, std::memory_order_release);
            EXPECT_EQ(fired->Cancel(), LLBC_OK);
            EXPECT_FALSE(fired->IsScheduled());
        },
        [&](LLBC_Timer *cancelled) {
            EXPECT_FALSE(cancelled->IsScheduled());
            EXPECT_TRUE(cancelled->IsHandlingTimeout());
            EXPECT_TRUE(cancelled->IsHandlingCancel());
            EXPECT_NE(cancelled->ToString().find("handlingTimeout:true"), static_cast<LLBC_String::size_type>(-1));
            EXPECT_NE(cancelled->ToString().find("handlingCancel:true"), static_cast<LLBC_String::size_type>(-1));
            cancelCount.fetch_add(1, std::memory_order_release);
            EXPECT_EQ(cancelled->Cancel(), LLBC_OK);
        },
        &scheduler);

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::zero), LLBC_OK);
    EXPECT_EQ(timer.GetTotalTriggerCount(), static_cast<size_t>(LLBC_INFINITE));

    scheduler.Update();
    EXPECT_EQ(timeoutCount.load(std::memory_order_acquire), 1);
    EXPECT_EQ(cancelCount.load(std::memory_order_acquire), 1);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_FALSE(timer.IsHandlingTimeout());
    EXPECT_FALSE(timer.IsHandlingCancel());
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);
}

// A finite timer with zero first-period and zero repeat-period is processed in
// the current Update() pass until its configured trigger count is exhausted.
TEST(TimerTest, ProcessesFiniteImmediateTimersInSingleUpdate)
{
    LLBC_TimerScheduler scheduler;
    int timeoutCount = 0;
    LLBC_Timer timer(
        [&](LLBC_Timer *fired) {
            ++timeoutCount;
            EXPECT_EQ(fired->GetTriggeredCount(), static_cast<size_t>(timeoutCount));
        },
        nullptr,
        &scheduler);

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::zero, LLBC_TimeSpan::zero, 2), LLBC_OK);
    scheduler.Update();

    EXPECT_EQ(timeoutCount, 2);
    EXPECT_EQ(timer.GetTriggeredCount(), 2lu);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);
}

// Rescheduling from a timeout handler replaces the active timer data rather
// than retaining the old periodic registration.
TEST(TimerTest, ReschedulesFromTimeoutWithFreshTimerData)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> timeoutCount {0};
    std::atomic<int> cancelCount {0};
    LLBC_TimerId firstTimerId = LLBC_INVALID_TIMER_ID;
    LLBC_Timer timer(
        [&](LLBC_Timer *fired) {
            const int count = timeoutCount.fetch_add(1, std::memory_order_acq_rel) + 1;
            if (count != 1)
                return;

            firstTimerId = fired->GetTimerId();
            EXPECT_TRUE(fired->IsScheduled());
            EXPECT_TRUE(fired->IsHandlingTimeout());
            ASSERT_EQ(fired->Schedule(LLBC_TimeSpan::FromMillis(2),
                                      LLBC_TimeSpan::zero,
                                      1),
                      LLBC_OK);
            EXPECT_NE(fired->GetTimerId(), firstTimerId);
            EXPECT_TRUE(fired->IsScheduled());
            EXPECT_FALSE(fired->IsHandlingTimeout());
        },
        [&](LLBC_Timer *cancelled) {
            EXPECT_FALSE(cancelled->IsScheduled());
            EXPECT_TRUE(cancelled->IsHandlingTimeout());
            EXPECT_TRUE(cancelled->IsHandlingCancel());
            cancelCount.fetch_add(1, std::memory_order_release);
        },
        &scheduler);

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::zero, LLBC_TimeSpan::zero, 1), LLBC_OK);
    ASSERT_TRUE(DriveUntil(scheduler, timeoutCount, 2));
    scheduler.Update();

    EXPECT_EQ(timeoutCount.load(std::memory_order_acquire), 2);
    EXPECT_EQ(cancelCount.load(std::memory_order_acquire), 1);
    EXPECT_EQ(timer.GetTriggeredCount(), 1lu);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);
}

// A cancel callback may schedule a replacement. The scheduler must retain the
// replacement while lazily removing the cancelled heap entry on Update().
TEST(TimerTest, ReschedulesFromCancelHandlerAndPrunesCancelledHeapEntry)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> timeoutCount {0};
    std::atomic<int> cancelCount {0};
    bool rescheduled = false;
    LLBC_Timer timer(
        [&](LLBC_Timer *) {
            timeoutCount.fetch_add(1, std::memory_order_release);
        },
        [&](LLBC_Timer *cancelled) {
            EXPECT_FALSE(cancelled->IsScheduled());
            EXPECT_FALSE(cancelled->IsHandlingTimeout());
            EXPECT_TRUE(cancelled->IsHandlingCancel());
            cancelCount.fetch_add(1, std::memory_order_release);
            EXPECT_EQ(cancelled->Cancel(), LLBC_OK);

            if (!rescheduled)
            {
                rescheduled = true;
                EXPECT_EQ(cancelled->Schedule(LLBC_TimeSpan::zero,
                                              LLBC_TimeSpan::zero,
                                              1),
                          LLBC_OK);
                EXPECT_TRUE(cancelled->IsScheduled());
            }
        },
        &scheduler);

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::zero), LLBC_OK);
    const LLBC_TimerId originalTimerId = timer.GetTimerId();
    ASSERT_EQ(timer.Cancel(), LLBC_OK);
    EXPECT_TRUE(rescheduled);
    EXPECT_TRUE(timer.IsScheduled());
    EXPECT_NE(timer.GetTimerId(), originalTimerId);

    ASSERT_TRUE(DriveUntil(scheduler, timeoutCount, 1));
    scheduler.Update();
    EXPECT_EQ(timeoutCount.load(std::memory_order_acquire), 1);
    EXPECT_EQ(cancelCount.load(std::memory_order_acquire), 1);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);
}

// CancelAll invokes every cancel callback, blocks re-scheduling until the
// batch completes, and lets the next Update() discard cancelled entries.
TEST(TimerTest, CancelAllRejectsSchedulingFromCancelCallbacks)
{
    LLBC_TimerScheduler scheduler;
    int firstCancelCount = 0;
    int secondCancelCount = 0;
    int nestedScheduleRet = LLBC_OK;
    int nestedScheduleError = LLBC_ERROR_SUCCESS;
    LLBC_Timer deferredTimer(nullptr, nullptr, &scheduler);
    LLBC_Timer firstTimer(
        nullptr,
        [&](LLBC_Timer *cancelled) {
            ++firstCancelCount;
            EXPECT_FALSE(cancelled->IsScheduled());
            EXPECT_TRUE(cancelled->IsHandlingCancel());
            scheduler.CancelAll();
            nestedScheduleRet = deferredTimer.Schedule(LLBC_TimeSpan::zero);
            nestedScheduleError = LLBC_GetLastError();
        },
        &scheduler);
    LLBC_Timer secondTimer(
        nullptr,
        [&](LLBC_Timer *cancelled) {
            ++secondCancelCount;
            EXPECT_FALSE(cancelled->IsScheduled());
            EXPECT_TRUE(cancelled->IsHandlingCancel());
        },
        &scheduler);

    ASSERT_EQ(firstTimer.Schedule(LLBC_TimeSpan::zero), LLBC_OK);
    ASSERT_EQ(secondTimer.Schedule(LLBC_TimeSpan::zero), LLBC_OK);
    ASSERT_EQ(scheduler.GetTimerCount(), 2lu);

    scheduler.CancelAll();
    EXPECT_EQ(firstCancelCount, 1);
    EXPECT_EQ(secondCancelCount, 1);
    EXPECT_EQ(nestedScheduleRet, LLBC_FAILED);
    EXPECT_EQ(nestedScheduleError, LLBC_ERROR_TIMER_SCHEDULER_CANCELING_ALL);
    EXPECT_FALSE(firstTimer.IsScheduled());
    EXPECT_FALSE(secondTimer.IsScheduled());

    scheduler.Update();
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);
}

// A long timeout is removed from the scheduler heap immediately on cancellation.
// Scheduling again replaces the retained timer data with a new registration.
TEST(TimerTest, RemovesLongCancelledTimerBeforeSchedulingReplacement)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> timeoutCount {0};
    LLBC_Timer timer(
        [&](LLBC_Timer *) {
            timeoutCount.fetch_add(1, std::memory_order_release);
        },
        nullptr,
        &scheduler);

    const LLBC_TimeSpan longTimeout = LLBC_TimeSpan::FromMillis(
        static_cast<sint64>(LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME) + 1000);
    ASSERT_EQ(timer.Schedule(longTimeout), LLBC_OK);
    const LLBC_TimerId longTimerId = timer.GetTimerId();
    ASSERT_EQ(timer.Cancel(), LLBC_OK);
    EXPECT_FALSE(timer.IsScheduled());
    EXPECT_EQ(timer.GetFirstPeriod(), longTimeout);
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);

    ASSERT_EQ(timer.Schedule(LLBC_TimeSpan::zero, LLBC_TimeSpan::zero, 1), LLBC_OK);
    EXPECT_NE(timer.GetTimerId(), longTimerId);
    scheduler.Update();
    EXPECT_EQ(timeoutCount.load(std::memory_order_acquire), 1);
    EXPECT_FALSE(timer.IsScheduled());
}

// Timers may be destroyed from a cancel override. Long-timeout cancellation
// removes the heap node immediately, so the scheduler must not access the
// deleted timer after invoking the callback.
TEST(TimerTest, SupportsSelfDeletionFromLongTimeoutCancelOverride)
{
    LLBC_TimerScheduler scheduler;
    std::atomic<int> cancelCount {0};
    auto *timer = new SelfDeletingTimer(&scheduler, &cancelCount);
    const LLBC_TimeSpan longTimeout = LLBC_TimeSpan::FromMillis(
        static_cast<sint64>(LLBC_CFG_CORE_TIMER_LONG_TIMEOUT_TIME) + 1000);

    ASSERT_EQ(timer->Schedule(longTimeout), LLBC_OK);
    EXPECT_EQ(timer->Cancel(), LLBC_OK);
    EXPECT_EQ(cancelCount.load(std::memory_order_acquire), 1);
    EXPECT_EQ(scheduler.GetTimerCount(), 0lu);
}

// Scheduler teardown cancels still-scheduled timers. A callback cannot create
// a new registration once destruction has started, but the timer object remains
// safe to destroy after the scheduler leaves scope.
TEST(TimerTest, SchedulerDestructionCancelsLiveTimersAndBlocksReschedule)
{
    std::atomic<int> cancelCount {0};
    int rescheduleRet = LLBC_OK;
    int rescheduleError = LLBC_ERROR_SUCCESS;
    std::unique_ptr<LLBC_Timer> timer;

    {
        LLBC_TimerScheduler scheduler;
        timer.reset(new LLBC_Timer(
            nullptr,
            [&](LLBC_Timer *cancelled) {
                EXPECT_FALSE(cancelled->IsScheduled());
                EXPECT_FALSE(cancelled->IsHandlingTimeout());
                EXPECT_TRUE(cancelled->IsHandlingCancel());
                cancelCount.fetch_add(1, std::memory_order_release);
                rescheduleRet = cancelled->Schedule(LLBC_TimeSpan::zero);
                rescheduleError = LLBC_GetLastError();
            },
            &scheduler));

        EXPECT_TRUE(scheduler.IsEnabled());
        ASSERT_EQ(timer->Schedule(LLBC_TimeSpan::zero), LLBC_OK);
        EXPECT_EQ(scheduler.GetTimerCount(), 1lu);
    }

    EXPECT_EQ(cancelCount.load(std::memory_order_acquire), 1);
    EXPECT_EQ(rescheduleRet, LLBC_FAILED);
    EXPECT_EQ(rescheduleError, LLBC_ERROR_TIMER_SCHEDULER_DESTROYING);
    EXPECT_FALSE(timer->IsScheduled());
    timer.reset();
}
