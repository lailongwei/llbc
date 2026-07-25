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
// @coverage-target: llbc/src/core/thread/RWLock.cpp
// @coverage-target: llbc/src/core/thread/RecursiveLock.cpp
// @coverage-target: llbc/src/core/thread/SpinLock.cpp
// @coverage-target: llbc/src/core/thread/FastLock.cpp
// @coverage-target: llbc/src/core/thread/Guard.cpp
// @coverage-target: llbc/include/llbc/core/thread/GuardInl.h

namespace
{

bool WaitFor(const std::atomic<bool> &value)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (!value.load(std::memory_order_acquire))
    {
        if (std::chrono::steady_clock::now() >= deadline)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return true;
}

template <typename LockType>
void VerifyNonRecursiveLock(LockType &lock)
{
    EXPECT_FALSE(lock.IsDummyLock());
    EXPECT_TRUE(lock.TryLock());

    std::atomic<bool> otherThreadCouldLock {true};
    std::thread contender([&] {
        otherThreadCouldLock.store(lock.TryLock(), std::memory_order_release);
        if (otherThreadCouldLock.load(std::memory_order_acquire))
            lock.Unlock();
    });
    contender.join();
    EXPECT_FALSE(otherThreadCouldLock.load(std::memory_order_acquire));

    lock.Unlock();
    EXPECT_TRUE(lock.TryLock());
    lock.Unlock();
}

struct GuardTrackedObject
{
    ~GuardTrackedObject()
    {
        ++destructions;
    }

    static inline int destructions = 0;
};

struct GuardMemberReceiver
{
    void Add(int value)
    {
        sum += value;
    }

    int sum = 0;
};

} // namespace

// Recursive locks allow a single owner to acquire repeatedly and release the
// matching number of times, unlike the simple, fast, and spin lock variants.
TEST(LockTest, RecursiveLockSupportsReentrantAcquisition)
{
    LLBC_RecursiveLock lock;
    EXPECT_FALSE(lock.IsDummyLock());
    lock.Lock();
    lock.Lock();
    EXPECT_TRUE(lock.TryLock());
    lock.Unlock();
    lock.Unlock();
    lock.Unlock();
    EXPECT_TRUE(lock.TryLock());
    lock.Unlock();
}

TEST(LockTest, SpinAndFastLocksExcludeOtherThreads)
{
    LLBC_SpinLock spinLock;
    VerifyNonRecursiveLock(spinLock);

    LLBC_FastLock fastLock;
    VerifyNonRecursiveLock(fastLock);
}

// FastLock also exposes a blocking Lock() path on platforms where its normal
// fast path is backed by a mutex. A waiting caller must enter only after the
// current owner releases the lock.
TEST(LockTest, FastLockBlockingLockWaitsForOwnerRelease)
{
    LLBC_FastLock lock;
    lock.Lock();

    std::atomic<bool> waiterEntered {false};
    std::thread waiter([&] {
        lock.Lock();
        waiterEntered.store(true, std::memory_order_release);
        lock.Unlock();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    EXPECT_FALSE(waiterEntered.load(std::memory_order_acquire));
    lock.Unlock();
    ASSERT_TRUE(WaitFor(waiterEntered));
    waiter.join();
}

// Read/write locks allow concurrent readers but exclude writers. Waiting reader
// and writer paths are exercised with coordinated worker threads.
TEST(LockTest, ReadWriteLockCoordinatesReadersAndWriters)
{
    LLBC_RWLock lock;
    lock.ReadLock();
    EXPECT_TRUE(lock.ReadTryLock());
    EXPECT_FALSE(lock.WriteTryLock());
    lock.ReadUnlock();
    lock.ReadUnlock();

    EXPECT_TRUE(lock.WriteTryLock());
    EXPECT_FALSE(lock.ReadTryLock());
    lock.WriteUnlock();

    std::atomic<bool> readerEntered {false};
    lock.WriteLock();
    std::thread reader([&] {
        lock.ReadLock();
        readerEntered.store(true, std::memory_order_release);
        lock.ReadUnlock();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(readerEntered.load(std::memory_order_acquire));
    lock.WriteUnlock();
    ASSERT_TRUE(WaitFor(readerEntered));
    reader.join();

    std::atomic<bool> writerEntered {false};
    lock.ReadLock();
    std::thread writer([&] {
        lock.WriteLock();
        writerEntered.store(true, std::memory_order_release);
        lock.WriteUnlock();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(writerEntered.load(std::memory_order_acquire));
    lock.ReadUnlock();
    ASSERT_TRUE(WaitFor(writerEntered));
    writer.join();
}

// A writer queued behind an active writer takes the writer-notification path
// when the owner releases. This preserves writer progress instead of waking
// readers first and leaving the queued writer blocked indefinitely.
TEST(LockTest, ReadWriteLockWakesQueuedWriterAfterWriterRelease)
{
    LLBC_RWLock lock;
    lock.WriteLock();

    std::atomic<bool> writerAttempting {false};
    std::atomic<bool> writerEntered {false};
    std::thread writer([&] {
        writerAttempting.store(true, std::memory_order_release);
        lock.WriteLock();
        writerEntered.store(true, std::memory_order_release);
        lock.WriteUnlock();
    });

    ASSERT_TRUE(WaitFor(writerAttempting));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    EXPECT_FALSE(writerEntered.load(std::memory_order_acquire));
    lock.WriteUnlock();
    ASSERT_TRUE(WaitFor(writerEntered));
    writer.join();
}

// Scope guards pair locking and cleanup operations even in early-return paths.
TEST(LockTest, GuardsLockUnlockDeleteFreeAndInvokeAtScopeExit)
{
    LLBC_SimpleLock lock;
    {
        LLBC_LockGuard guard(lock);
        EXPECT_FALSE(lock.TryLock());
    }
    EXPECT_TRUE(lock.TryLock());
    lock.Unlock();

    lock.Lock();
    {
        LLBC_LockGuard reverseGuard(lock, true);
        EXPECT_TRUE(lock.TryLock());
        lock.Unlock();
    }
    EXPECT_FALSE(lock.TryLock());
    lock.Unlock();

    int *single = new int(1);
    {
        LLBC_DeleteGuard<int> guard(single);
    }
    EXPECT_EQ(single, nullptr);

    int *array = new int[2] {1, 2};
    {
        LLBC_DeletesGuard<int> guard(array);
    }
    EXPECT_EQ(array, nullptr);

    int *memory = LLBC_Malloc(int, 2);
    {
        LLBC_FreeGuard<int> guard(memory);
    }
    EXPECT_EQ(memory, nullptr);

    int invoked = 0;
    {
        LLBC_InvokeGuard guard([&] { ++invoked; });
        EXPECT_EQ(invoked, 0);
    }
    EXPECT_EQ(invoked, 1);
}

// Callers can retain their pointer variables after a guard releases storage.
// This is useful when the pointer is embedded in a larger ownership record;
// destruction still happens exactly once, but nulling is intentionally opted
// out. Member callback guards follow the same scope-exit contract.
TEST(LockTest, GuardsSupportNonNullingCleanupAndMemberCallbacks)
{
    GuardTrackedObject::destructions = 0;
    GuardTrackedObject *single = new GuardTrackedObject;
    {
        LLBC_DeleteGuard<GuardTrackedObject> guard(single, false);
    }
    EXPECT_EQ(GuardTrackedObject::destructions, 1);
    single = nullptr;

    GuardTrackedObject::destructions = 0;
    GuardTrackedObject *array = new GuardTrackedObject[2];
    {
        LLBC_DeletesGuard<GuardTrackedObject> guard(array, false);
    }
    EXPECT_EQ(GuardTrackedObject::destructions, 2);
    array = nullptr;

    int *memory = LLBC_Malloc(int, 2);
    {
        LLBC_FreeGuard<int> guard(memory, false);
    }
    memory = nullptr;

    GuardMemberReceiver receiver;
    {
        LLBC_InvokeGuard guard(&receiver, &GuardMemberReceiver::Add, 7);
        EXPECT_EQ(receiver.sum, 0);
    }
    EXPECT_EQ(receiver.sum, 7);
}
