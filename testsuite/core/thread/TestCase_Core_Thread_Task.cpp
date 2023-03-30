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

/**
 * \brief Test task encapsulation.
 */
class TestTask : public LLBC_Task
{
public:
    explicit TestTask(size_t perThreadPopTimes);
    virtual ~TestTask();
    
public:
    virtual void Svc();

    virtual void Cleanup();

private:
    const size_t _perThreadPopTimes;
    static LLBC_THREAD_LOCAL int *_val;
};

LLBC_THREAD_LOCAL int *TestTask::_val = nullptr;

inline TestTask::TestTask(size_t perThreadPopTimes)
: _perThreadPopTimes(perThreadPopTimes)
{
}

inline TestTask::~TestTask()
{
}

inline void TestTask::Svc()
{
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

        LLBC_PrintLn("[%d] fetch data: %s",  LLBC_GetCurrentThreadId(), content.c_str());

        delete block;
    }

    return;
}

inline void TestTask::Cleanup()
{
    LLBC_PrintLn("Task cleanup, queue size:%lu", GetMessageSize());
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

    // Activate task.
    const int threadNum = 5;
    const size_t pushMsgSize = 10000 * threadNum;
    TestTask *task = new TestTask(pushMsgSize / threadNum);
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
        block->Write(stream.GetBuf(), stream.GetPos());

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

    return 0;
}
