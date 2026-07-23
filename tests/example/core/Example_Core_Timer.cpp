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

#include "core/Example_Core_Timer.h"

int Example_Core_Timer::Run(int argc, char *argv[])
{
    // The entry thread already created a timer scheduler during LLBC_Startup(),
    // and a default-constructed LLBC_Timer auto-attaches to the current thread's scheduler.
    LLBC_TimerScheduler *scheduler = LLBC_TimerScheduler::GetCurrentThreadScheduler();
    LLBC_PrintLn("Timer scheduler for the current thread: %p", scheduler);

    // ========================= Periodic timeout timer =========================
    // firstPeriod = first trigger interval, period = subsequent trigger interval, triggerCount = total trigger count.
    LLBC_PrintLn("Scheduling a repeating timer: first tick after 50 ms, then every 50 ms, three ticks total");
    LLBC_Timer periodTimer([](LLBC_Timer *timer) {
        // Timeout callback: print triggered count / total count.
        LLBC_PrintLn("- [timeout] periodTimer fired, tick %zu/%zu",
                     timer->GetTriggeredCount(), timer->GetTotalTriggerCount());
    });
    periodTimer.Schedule(LLBC_TimeSpan::FromMillis(50), LLBC_TimeSpan::FromMillis(50), 3);

    // ========================= Cancellable timer =========================
    // Set up a timer that fires later (1s), then actively Cancel() it to demonstrate the cancel callback.
    LLBC_PrintLn("Scheduling a timer for one second later, then cancelling it");
    LLBC_Timer cancelTimer(
        [](LLBC_Timer *timer) {
            // Normally this won't be called (it gets cancelled first).
            LLBC_PrintLn("- [timeout] cancelTimer fired (should not happen)");
        },
        [](LLBC_Timer *timer) {
            // Cancel callback.
            LLBC_PrintLn("- [cancel] cancelTimer cancelled, timerId:%llu", timer->GetTimerId());
        });
    cancelTimer.Schedule(LLBC_TimeSpan::oneSec);
    LLBC_PrintLn("cancelTimer scheduled: %s", cancelTimer.IsScheduled() ? "true" : "false");

    // ========================= Drive the scheduler =========================
    // Timers don't fire automatically; you must periodically call scheduler->Update() to drive them.
    // Here a bounded loop (Sleep + Update) simulates the main loop, exiting after about 300ms.
    LLBC_PrintLn("Driving the scheduler (bounded loop, about 300 ms)...");
    for (int i = 0; i < 30; ++i)
    {
        LLBC_Sleep(10); // Sleep 10ms each iteration
        scheduler->Update();

        // Cancel cancelTimer halfway through the loop, triggering its cancel callback.
        if (i == 15 && cancelTimer.IsScheduled())
        {
            LLBC_PrintLn("Cancelling cancelTimer explicitly...");
            cancelTimer.Cancel();
        }
    }

    // After the loop, stop the periodic timer (if still scheduled) to ensure a clean exit.
    periodTimer.Cancel();
    LLBC_PrintLn("Scheduler drive complete; remaining timers: %zu", scheduler->GetTimerCount());

    return LLBC_OK;
}
