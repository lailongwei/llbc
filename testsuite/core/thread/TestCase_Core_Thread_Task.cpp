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
    static LLBC_THREAD_LOCAL int *_val;
};

LLBC_THREAD_LOCAL int *BasicTestTask::_val = nullptr;

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

