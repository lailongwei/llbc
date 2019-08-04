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
class TestTask : public LLBC_BaseTask
{
public:
    TestTask();
    virtual ~TestTask();
    
public:
    virtual void Svc();

    virtual void Cleanup();

private:
    volatile int _repeatCount;
    static LLBC_THREAD_LOCAL int *_val;
};

int *TestTask::_val = NULL;

inline TestTask::TestTask()
    : _repeatCount(10000)
{
}

inline TestTask::~TestTask()
{
}

inline void TestTask::Svc()
{
    _val = new int();
    LLBC_PrintLine("Thread allocated thread-local variable[int *], ptr:%p", _val);
    for (int i = 0; i < 100000; ++i)
        *_val += 1;
    LLBC_PrintLine("Exec `*val += 1` finished, val:%d", *_val);
    LLBC_XDelete(_val);

    LLBC_MessageBlock *block = NULL;
    while(--_repeatCount > 0)
    {
        Pop(block);
        LLBC_String content;
        LLBC_Stream stream(block->GetData(), block->GetReadableSize());
        stream.Read(content);

        LLBC_PrintLine("task fetch data: %s", content.c_str());

        LLBC_Delete(block);
    }

    return;
}

inline void TestTask::Cleanup()
{
    LLBC_PrintLine("Task cleanup!!!!!!!!!!");
}

TestCase_Core_Thread_Task::TestCase_Core_Thread_Task()
{
}

TestCase_Core_Thread_Task::~TestCase_Core_Thread_Task()
{
}

int TestCase_Core_Thread_Task::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/thread/task test:");

    // Activate task.
    TestTask *task = LLBC_New0(TestTask);
    task->Activate(2);

    // Send message to task.
    for(int i = 0; i < 50000; ++i)
    {
        LLBC_Stream stream;
        LLBC_String content = "Hello Tasks!";
        stream.Write(content);
        LLBC_MessageBlock *block = LLBC_New0(LLBC_MessageBlock);
        block->Write(stream.GetBuf(), stream.GetPos());

        task->Push(block);
    }

    // Wait task.
    task->Wait();
    // Delete task.
    LLBC_Delete(task);

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
