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
#include <thread>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/thread/ConditionVariable.cpp
// @coverage-target: llbc/src/core/thread/SimpleLock.cpp

namespace
{

bool WaitUntil(const std::atomic<int> &value, int expected)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (value.load(std::memory_order_acquire) != expected)
    {
        if (std::chrono::steady_clock::now() >= deadline)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return true;
}

} // namespace

// Timed waits preserve the caller's lock ownership and distinguish invalid
// negative waits from normal timeout behavior.
TEST(ConditionVariableTest, RejectsInvalidTimeoutAndReportsTimeout)
{
    LLBC_ConditionVariable condition;
    LLBC_SimpleLock lock;

    lock.Lock();
    EXPECT_EQ(condition.TimedWait(lock, -2), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(condition.TimedWait(lock, 20), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_TIMEOUTED);
    lock.Unlock();
}

// Notify releases exactly one waiter after the shared predicate is made true.
TEST(ConditionVariableTest, NotifyUnblocksAWaitingThread)
{
    LLBC_ConditionVariable condition;
    LLBC_SimpleLock lock;
    std::atomic<int> waiters {0};
    std::atomic<int> awakened {0};
    std::atomic<bool> ready {false};

    std::thread worker([&] {
        lock.Lock();
        waiters.fetch_add(1, std::memory_order_release);
        while (!ready.load(std::memory_order_acquire))
            condition.Wait(lock);
        awakened.fetch_add(1, std::memory_order_release);
        lock.Unlock();
    });

    ASSERT_TRUE(WaitUntil(waiters, 1));
    lock.Lock();
    ready.store(true, std::memory_order_release);
    lock.Unlock();
    condition.Notify();

    worker.join();
    EXPECT_EQ(awakened.load(std::memory_order_acquire), 1);
}

// Timed waits share the same predicate discipline as indefinite waits, but
// must return success rather than timeout when a notifier arrives before the
// deadline.
TEST(ConditionVariableTest, TimedWaitSucceedsWhenNotifierArrivesBeforeDeadline)
{
    LLBC_ConditionVariable condition;
    LLBC_SimpleLock lock;
    std::atomic<int> waiters {0};
    std::atomic<bool> completed {false};
    std::atomic<int> waitResult {LLBC_FAILED};
    std::atomic<bool> ready {false};

    std::thread worker([&] {
        lock.Lock();
        waiters.fetch_add(1, std::memory_order_release);
        while (!ready.load(std::memory_order_acquire))
        {
            const int result = condition.TimedWait(lock, 500);
            waitResult.store(result, std::memory_order_release);
            if (result != LLBC_OK)
                break;
        }
        completed.store(ready.load(std::memory_order_acquire), std::memory_order_release);
        lock.Unlock();
    });

    ASSERT_TRUE(WaitUntil(waiters, 1));
    lock.Lock();
    ready.store(true, std::memory_order_release);
    lock.Unlock();
    condition.Notify();

    worker.join();
    EXPECT_EQ(waitResult.load(std::memory_order_acquire), LLBC_OK);
    EXPECT_TRUE(completed.load(std::memory_order_acquire));
}

// Broadcast must release all waiters, which is the coordination pattern used by
// shutdown and global state transitions.
TEST(ConditionVariableTest, BroadcastUnblocksAllWaitingThreads)
{
    LLBC_ConditionVariable condition;
    LLBC_SimpleLock lock;
    std::atomic<int> waiters {0};
    std::atomic<int> awakened {0};
    std::atomic<bool> ready {false};

    auto worker = [&] {
        lock.Lock();
        waiters.fetch_add(1, std::memory_order_release);
        while (!ready.load(std::memory_order_acquire))
            condition.Wait(lock);
        awakened.fetch_add(1, std::memory_order_release);
        lock.Unlock();
    };

    std::thread first(worker);
    std::thread second(worker);
    ASSERT_TRUE(WaitUntil(waiters, 2));
    lock.Lock();
    ready.store(true, std::memory_order_release);
    lock.Unlock();
    condition.Broadcast();

    first.join();
    second.join();
    EXPECT_EQ(awakened.load(std::memory_order_acquire), 2);
}

// Basic mutex semantics underpin condition-variable use: the non-recursive
// simple lock must expose TryLock and IsDummyLock consistently.
TEST(ConditionVariableTest, SimpleLockExposesExpectedMutexBehavior)
{
    LLBC_SimpleLock lock;
    EXPECT_FALSE(lock.IsDummyLock());
    EXPECT_TRUE(lock.TryLock());
    lock.Unlock();

    lock.Lock();
    EXPECT_FALSE(lock.TryLock());
    lock.Unlock();
}
