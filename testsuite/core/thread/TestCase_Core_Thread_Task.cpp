/**
 * @file    TestCase_Core_Thread_Task.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/01
 * @version 1.0
 *
 * @brief
 */

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
    int m_repeatCount;
};

inline TestTask::TestTask()
    : m_repeatCount(10000)
{
}

inline TestTask::~TestTask()
{
}

inline void TestTask::Svc()
{
    LLBC_MessageBlock *block = NULL;
    while(-- m_repeatCount > 0)
    {
        Pop(block);
        LLBC_String content;
        LLBC_Stream stream(block->GetData(), block->GetReadableSize());
        stream.Read(content);

        LLBC_PrintLine("task fetch data: %s", content.c_str());

        delete block;
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
    TestTask *task = new TestTask;
    task->Activate(2);

    // Send message to task.
    for(int i = 0; i < 50000; i ++)
    {
        LLBC_Stream stream;
        LLBC_String content = "Hello Tasks!";
        stream.Write(content);
        LLBC_MessageBlock *block = new LLBC_MessageBlock;
        block->Write(stream.GetBuf(), stream.GetPos());

        task->Push(block);
    }

    // Wait task.
    task->Wait();
    // Delete task.
    delete task;

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
