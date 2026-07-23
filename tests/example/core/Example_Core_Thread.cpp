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

#include "core/Example_Core_Thread.h"

namespace
{
    // A simple sample task: pop a fixed number of messages from the message queue, then finish.
    // LLBC_Task has two pure virtual functions that must be implemented: Svc() (worker thread body) and Cleanup() (teardown).
    class DemoTask final : public LLBC_Task
    {
    public:
        explicit DemoTask(size_t msgCount) : _msgCount(msgCount) {}

        // Svc() runs in each worker thread; this sample uses only 1 thread.
        void Svc() override
        {
            LLBC_PrintLn("- [worker] Svc() started, threadId:%d, messages to process:%zu",
                         LLBC_GetCurrentThreadId(), _msgCount);

            LLBC_MessageBlock *block = nullptr;
            for (size_t i = 0; i < _msgCount; ++i)
            {
                // Pop() blocks to take one message block; it waits if none is available.
                Pop(block);

                LLBC_String content;
                LLBC_Stream stream(block->GetData(), block->GetReadableSize());
                stream.Read(content);
                LLBC_PrintLn("- [worker] Received message: %s", content.c_str());

                delete block;
            }

            LLBC_PrintLn("- [worker] Svc() processing complete; returning");
        }

        // Called after all worker threads finish, for teardown.
        void Cleanup() override
        {
            LLBC_PrintLn("- [worker] Cleanup() invoked, remaining queue size:%zu", GetMessageSize());
        }

    private:
        const size_t _msgCount;
    };
}

int Example_Core_Thread::Run(int argc, char *argv[])
{
    const size_t msgCount = 5;

    // Create and activate the task (1 worker thread).
    LLBC_PrintLn("Creating DemoTask and calling Activate() with one thread");
    DemoTask *task = new DemoTask(msgCount);
    task->Activate(1);
    LLBC_PrintLn("Task state: %s", LLBC_TaskState::GetDesc(task->GetTaskState()));

    // Push several messages to the task, to be consumed by the worker thread.
    LLBC_PrintLn("Main thread pushing %zu messages", msgCount);
    for (size_t i = 0; i < msgCount; ++i)
    {
        LLBC_Stream stream;
        stream.Write(LLBC_String().format("Message #%zu", i));

        LLBC_MessageBlock *block = new LLBC_MessageBlock;
        block->Write(stream.GetBuf(), stream.GetWritePos());
        task->Push(block);
    }

    // Wait() blocks until every worker thread's Svc() returns and Cleanup() completes.
    LLBC_PrintLn("Main thread calling Wait() for task completion...");
    task->Wait();
    LLBC_PrintLn("Task completed, final state: %s", LLBC_TaskState::GetDesc(task->GetTaskState()));

    delete task;

    return LLBC_OK;
}
