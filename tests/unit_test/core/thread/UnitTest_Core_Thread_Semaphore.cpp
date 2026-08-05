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
// @coverage-target: llbc/src/core/thread/Semaphore.cpp

// Semaphores provide counting permits. Try/timed waits must not consume a
// nonexistent permit, while nonpositive post counts still contribute one permit.
TEST(SemaphoreTest, SupportsTryTimedAndCountingPermits)
{
    LLBC_Semaphore semaphore;
    EXPECT_FALSE(semaphore.TryWait());
    EXPECT_FALSE(semaphore.TimedWait(0));

    semaphore.Post(-1);
    EXPECT_TRUE(semaphore.TryWait());
    EXPECT_FALSE(semaphore.TryWait());

    semaphore.Post(2);
    EXPECT_TRUE(semaphore.TryWait());
    EXPECT_TRUE(semaphore.TryWait());
    EXPECT_FALSE(semaphore.TryWait());

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_FALSE(semaphore.TimedWait(20));
    EXPECT_NE(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);
}

// A waiter in another thread must unblock exactly after a post, establishing the
// synchronization scenario used by worker queues and timers.
TEST(SemaphoreTest, WaitUnblocksAfterAnotherThreadPosts)
{
    LLBC_Semaphore semaphore;
    std::atomic<bool> acquired {false};

    std::thread waiter([&] {
        semaphore.Wait();
        acquired.store(true, std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(acquired.load(std::memory_order_acquire));
    semaphore.Post();
    waiter.join();
    EXPECT_TRUE(acquired.load(std::memory_order_acquire));
}

// Constructor permits are immediately available and must be consumed in order.
TEST(SemaphoreTest, HonorsInitialPermitCount)
{
    LLBC_Semaphore semaphore(2);
    EXPECT_TRUE(semaphore.TryWait());
    EXPECT_TRUE(semaphore.TryWait());
    EXPECT_FALSE(semaphore.TryWait());
}

// TimedWait must consume both a permit that is already present and one posted
// while it is polling/waiting. These are distinct success paths on macOS.
TEST(SemaphoreTest, TimedWaitConsumesImmediateAndDelayedPermits)
{
    LLBC_Semaphore semaphore;
    semaphore.Post();
    EXPECT_TRUE(semaphore.TimedWait(50));

    std::thread poster([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        semaphore.Post();
    });
    EXPECT_TRUE(semaphore.TimedWait(100));
    poster.join();
    EXPECT_FALSE(semaphore.TryWait());
}
