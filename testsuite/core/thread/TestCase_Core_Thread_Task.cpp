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

#include <thread>

#include "core/thread/TestCase_Core_Thread_Task.h"

namespace
{

/**
 * \brief Test task encapsulation.
 */
class BasicTestTask final : public LLBC_Task
{
public:
    explicit BasicTestTask(size_t perThreadPopTimes);
    ~BasicTestTask() override = default;

public:
    void Svc() override;

    void Cleanup() override;

private:
    const size_t _perThreadPopTimes;
    static thread_local int *_val;
};

/**
 * \brief Test task encapsulation.
 */
class MultiThreadTestTask final : public LLBC_MultiThreadTask
{
public:
    void Svc() override;
    void Cleanup() override;
};

thread_local int *BasicTestTask::_val = nullptr;

BasicTestTask::BasicTestTask(size_t perThreadPopTimes)
: _perThreadPopTimes(perThreadPopTimes)
{
}

void BasicTestTask::Svc()
{
    ASSERT(GetTaskState() == LLBC_TaskState::Activated);

    _val = new int();
    LLBC_PrintLn("Thread allocated thread-local variable[int *], ptr:%p", _val);
    for (int i = 0; i < 100000; ++i)
        *_val += 1;
    LLBC_PrintLn("Exec `*val += 1` finished, val:%d", *_val);
    LLBC_XDelete(_val);

    LLBC_MessageBlock *block = nullptr;
    for (size_t i = 0; i < _perThreadPopTimes; ++i)
    {
        Pop(block);
        LLBC_String content;
        LLBC_Stream stream(block->GetData(), block->GetReadableSize());
        stream.Read(content);

        LLBC_PrintLn("[%d] fetch data: %s", LLBC_GetCurrentThreadId(), content.c_str());

        delete block;
    }

    ASSERT(GetTaskState() == LLBC_TaskState::Activated);
}

void BasicTestTask::Cleanup()
{
    LLBC_PrintLn("Task cleanup, queue size:%lu", GetMessageSize());
}

void MultiThreadTestTask::Svc()
{
    auto threadId = LLBC_GetCurrentThreadId();
    auto processorId = GetProcessorId();

    LLBC_PrintLn("MultiThreadTaskTest::Svc, threadId:%d", threadId);

    while (true) 
    {
        LLBC_MessageBlock *block;
        while (TimedPop(block, 20) == LLBC_OK)
        {
            LLBC_String content;
            LLBC_Stream stream(block->GetData(), block->GetReadableSize());
            stream.Read(content);

            LLBC_PrintLn("threadId [%d] processorId[%d] recv msg: %s", threadId, processorId, content.c_str());

            delete block;
        }
    }
}

void MultiThreadTestTask::Cleanup()
{
}

}

TestCase_Core_Thread_Task::TestCase_Core_Thread_Task()
{
}

TestCase_Core_Thread_Task::~TestCase_Core_Thread_Task()
{
}

int TestCase_Core_Thread_Task::Run(int argc, char *argv[])
{
    LLBC_PrintLn("core/thread/task test:");

    LLBC_ErrorAndReturnIf(BasicTaskTest() != LLBC_OK, LLBC_FAILED)
    LLBC_ErrorAndReturnIf(EmptyTaskTest() != LLBC_OK, LLBC_FAILED)
    LLBC_ErrorAndReturnIf(MultiThreadTaskTest() != LLBC_OK, LLBC_FAILED)

    return LLBC_OK;
}

int TestCase_Core_Thread_Task::BasicTaskTest()
{
    LLBC_PrintLn("Basic task test:");

    // Activate task.
    const int threadNum = 5;
    const size_t pushMsgSize = 10000 * threadNum;
    BasicTestTask *task = new BasicTestTask(pushMsgSize / threadNum);
    task->Activate(5);
    const int taskState = task->GetTaskState();
    LLBC_PrintLn("Task activated, task state:%s(%d)",
                   LLBC_TaskState::GetDesc(taskState), taskState);

    // Send message to task.
    for(size_t i = 0; i < pushMsgSize; ++i)
    {
        LLBC_Stream stream;
        stream.Write(LLBC_String().format("Hello Message, seq:%d", i));
        LLBC_MessageBlock *block = new LLBC_MessageBlock;
        block->Write(stream.GetBuf(), stream.GetWritePos());

        task->Push(block);
    }

    // Before wait task, print task state.
    LLBC_PrintLn("Before wait task, task state:%s",
                   LLBC_TaskState::GetDesc(task->GetTaskState()));

    // Wait task.
    task->Wait();
    // Dump message queue size.
    LLBC_PrintLn("After task finished, task state:%s, queue size:%lu",
                   LLBC_TaskState::GetDesc(task->GetTaskState()), task->GetMessageSize());
    // Delete task.
    delete task;

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return LLBC_OK;
}

int TestCase_Core_Thread_Task::EmptyTaskTest()
{
    LLBC_PrintLn("Empty task test:");

    class EmptyTestTask : public LLBC_Task
    {
    public:
        void Svc() override {  }
        void Cleanup() override {  }
    };

    static constexpr int threadNum = 20;
    LLBC_PrintLn("Activate EmptyTestTask, threadNum:%d", threadNum);
    EmptyTestTask *task = new EmptyTestTask;
    LLBC_Defer(delete task);
    task->Activate(threadNum);

    LLBC_PrintLn("Wait EmptyTestTask...");
    LLBC_ErrorAndReturnIf(task->Wait() != LLBC_OK, LLBC_FAILED);
    LLBC_PrintLn("EmptyTestTask wait finished!");

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return LLBC_OK;
}

int TestCase_Core_Thread_Task::MultiThreadTaskTest()
{
    LLBC_PrintLn("MultiThreadTaskTest test:");

    MultiThreadTestTask *task = new MultiThreadTestTask;
    LLBC_Defer(delete task);

    const auto threadNum = 3;
    task->Activate(threadNum);


    auto pushMsg = [&]() {
        LLBC_Random rand;
        for (int i = 0; i < 100; ++i)
        {
            auto hashId = rand.Rand(threadNum*2);

            LLBC_Stream stream;
            stream.Write(LLBC_String().format("seq:%d", hashId));
            LLBC_MessageBlock *block = new LLBC_MessageBlock;
            block->Write(stream.GetBuf(), stream.GetWritePos());
    
            task->Push(hashId, block);
        }
    };

    // 多线程push消息
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
        threads.emplace_back(pushMsg);

    for (auto &thread : threads)
        thread.join();

    LLBC_PrintLn("Wait MultiThreadTestTask...");
    LLBC_ErrorAndReturnIf(task->Wait() != LLBC_OK, LLBC_FAILED);
    LLBC_PrintLn("MultiThreadTestTask wait finished!");

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return LLBC_OK;
}

