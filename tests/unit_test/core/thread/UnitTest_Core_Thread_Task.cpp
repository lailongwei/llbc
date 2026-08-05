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
#include <string>
#include <thread>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/thread/Task.cpp
// @coverage-target: llbc/include/llbc/core/thread/TaskInl.h

namespace
{

class OneShotTask final : public LLBC_Task
{
public:
    explicit OneShotTask(LLBC_ThreadMgr *threadMgr = nullptr)
    : LLBC_Task(threadMgr)
    {
    }

    std::atomic<int> svcCalls {0};
    std::atomic<int> cleanupCalls {0};
    std::string consumed;

    void Svc() override
    {
        LLBC_MessageBlock *block = nullptr;
        if (TryPop(block) == LLBC_OK)
        {
            consumed.assign(block->GetReadableSize(), '\0');
            block->Read(consumed.data(), consumed.size());
            delete block;
        }

        svcCalls.fetch_add(1, std::memory_order_release);
    }

    void Cleanup() override
    {
        cleanupCalls.fetch_add(1, std::memory_order_release);
    }
};

class BlockingTask final : public LLBC_Task
{
public:
    LLBC_Semaphore gate;
    std::atomic<int> svcCalls {0};
    std::atomic<int> cleanupCalls {0};

    void Svc() override
    {
        svcCalls.fetch_add(1, std::memory_order_release);
        gate.Wait();
    }

    void Cleanup() override
    {
        cleanupCalls.fetch_add(1, std::memory_order_release);
    }

    void ReleaseWorker()
    {
        gate.Post();
    }
};

LLBC_MessageBlock *MakeTaskBlock(const char *text)
{
    const size_t size = ::strlen(text);
    auto *block = new LLBC_MessageBlock(size);
    block->Write(text, size);
    return block;
}

void DeleteTaskBlockChain(LLBC_MessageBlock *block)
{
    while (block)
    {
        LLBC_MessageBlock *next = block->GetNext();
        delete block;
        block = next;
    }
}

} // namespace

// Task exposes its internal queue before activation, which lets callers stage
// work safely. The helper APIs also report all lifecycle descriptions and honor
// an explicitly supplied thread manager.
TEST(TaskTest, SupportsQueueHelpersAndInjectedThreadManager)
{
    EXPECT_STREQ(LLBC_TaskState::GetDesc(LLBC_TaskState::Activating), "Activating");
    EXPECT_STREQ(LLBC_TaskState::GetDesc(LLBC_TaskState::Deactivating), "Deactivating");

    LLBC_ThreadMgr threadMgr;
    OneShotTask task(&threadMgr);
    EXPECT_EQ(task.GetThreadMgr(), &threadMgr);

    ASSERT_EQ(task.Push(MakeTaskBlock("front")), LLBC_OK);
    ASSERT_EQ(task.Push(MakeTaskBlock("back")), LLBC_OK);
    EXPECT_EQ(task.GetMessageSize(), 2lu);

    LLBC_MessageBlock *block = nullptr;
    ASSERT_EQ(task.Pop(block), LLBC_OK);
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(block->GetReadableSize(), 5lu);
    delete block;

    block = nullptr;
    ASSERT_EQ(task.TryPop(block), LLBC_OK);
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(block->GetReadableSize(), 4lu);
    delete block;
    EXPECT_EQ(task.GetMessageSize(), 0lu);

    block = nullptr;
    EXPECT_EQ(task.TryPop(block), LLBC_FAILED);
    EXPECT_EQ(task.TimedPop(block, 1), LLBC_FAILED);

    ASSERT_EQ(task.Push(MakeTaskBlock("one")), LLBC_OK);
    ASSERT_EQ(task.Push(MakeTaskBlock("two")), LLBC_OK);
    LLBC_MessageBlock *all = nullptr;
    ASSERT_EQ(task.PopAll(all), LLBC_OK);
    ASSERT_NE(all, nullptr);
    EXPECT_EQ(all->GetReadableSize(), 3lu);
    ASSERT_NE(all->GetNext(), nullptr);
    EXPECT_EQ(all->GetNext()->GetReadableSize(), 3lu);
    DeleteTaskBlockChain(all);
    EXPECT_EQ(task.PopAll(all), LLBC_FAILED);
}

// TimedPop delegates to the task queue's timed producer/consumer path and
// succeeds when another thread supplies a message before the deadline.
TEST(TaskTest, TimedPopConsumesMessagePostedBeforeDeadline)
{
    OneShotTask task;
    std::thread producer([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        task.Push(MakeTaskBlock("delayed"));
    });

    LLBC_MessageBlock *block = nullptr;
    const int timedPopResult = task.TimedPop(block, 200);
    producer.join();
    ASSERT_EQ(timedPopResult, LLBC_OK);
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(block->GetReadableSize(), 7lu);
    delete block;
    EXPECT_EQ(task.GetMessageSize(), 0lu);
}

// State descriptions, idle waits, message delivery, worker execution, and
// cleanup form the core Task lifecycle.
TEST(TaskTest, ActivatesConsumesMessageWaitsAndCleansUp)
{
    EXPECT_STREQ(LLBC_TaskState::GetDesc(LLBC_TaskState::NotActivated), "NotActivated");
    EXPECT_STREQ(LLBC_TaskState::GetDesc(LLBC_TaskState::Activated), "Activated");
    EXPECT_STREQ(LLBC_TaskState::GetDesc(999), "UnknownTaskState");

    OneShotTask task;
    EXPECT_FALSE(task.IsActivated());
    EXPECT_EQ(task.GetTaskState(), LLBC_TaskState::NotActivated);
    EXPECT_EQ(task.Wait(), LLBC_OK);
    EXPECT_EQ(task.Activate(0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(task.Activate(1, LLBC_ThreadPriority::End), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_FALSE(task.IsActivated());
    EXPECT_EQ(task.GetTaskState(), LLBC_TaskState::NotActivated);

    ASSERT_EQ(task.Push(MakeTaskBlock("task-message")), LLBC_OK);
    EXPECT_EQ(task.GetMessageSize(), 1lu);
    ASSERT_EQ(task.Activate(), LLBC_OK);
    EXPECT_EQ(task.Wait(), LLBC_OK);

    EXPECT_EQ(task.svcCalls.load(std::memory_order_acquire), 1);
    EXPECT_EQ(task.cleanupCalls.load(std::memory_order_acquire), 1);
    EXPECT_EQ(task.consumed, "task-message");
    EXPECT_FALSE(task.IsActivated());
    EXPECT_EQ(task.GetTaskState(), LLBC_TaskState::NotActivated);
    EXPECT_EQ(task.GetThreadGroupHandle(), LLBC_INVALID_HANDLE);
}

// A task that remains in Svc() rejects reentrant activation. Once released, the
// activating thread can Wait() for cleanup and return to NotActivated.
TEST(TaskTest, RejectsReentrantActivationAndWaitsForWorkerExit)
{
    BlockingTask task;
    ASSERT_EQ(task.Activate(), LLBC_OK);
    EXPECT_TRUE(task.IsActivated());
    EXPECT_EQ(task.Activate(), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_ALLOW);

    task.ReleaseWorker();
    EXPECT_EQ(task.Wait(), LLBC_OK);
    EXPECT_EQ(task.svcCalls.load(std::memory_order_acquire), 1);
    EXPECT_EQ(task.cleanupCalls.load(std::memory_order_acquire), 1);
    EXPECT_FALSE(task.IsActivated());
}

// Multi-worker tasks exercise the coordination path where the first worker
// waits for the other Svc() calls to leave. Only the activation thread may wait
// for completion, preventing an unrelated thread from stealing lifecycle control.
TEST(TaskTest, CoordinatesMultipleWorkersAndRejectsWaitFromAnotherThread)
{
    BlockingTask task;
    ASSERT_EQ(task.Activate(2), LLBC_OK);
    EXPECT_TRUE(task.IsActivated());
    EXPECT_NE(task.GetThreadGroupHandle(), LLBC_INVALID_HANDLE);

    std::atomic<int> otherWaitRet {LLBC_OK};
    std::atomic<int> otherWaitError {LLBC_ERROR_SUCCESS};
    std::thread otherThread([&] {
        otherWaitRet.store(task.Wait(), std::memory_order_release);
        otherWaitError.store(LLBC_GetLastError(), std::memory_order_release);
    });
    otherThread.join();
    EXPECT_EQ(otherWaitRet.load(std::memory_order_acquire), LLBC_FAILED);
    EXPECT_EQ(otherWaitError.load(std::memory_order_acquire), LLBC_ERROR_NOT_ALLOW);

    task.ReleaseWorker();
    task.ReleaseWorker();
    EXPECT_EQ(task.Wait(), LLBC_OK);
    EXPECT_EQ(task.svcCalls.load(std::memory_order_acquire), 2);
    EXPECT_EQ(task.cleanupCalls.load(std::memory_order_acquire), 1);
    EXPECT_FALSE(task.IsActivated());
}
