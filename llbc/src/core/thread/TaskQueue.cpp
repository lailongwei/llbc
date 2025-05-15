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

#include "llbc/common/Export.h"

#include "llbc/core/thread/TaskQueue.h"

__LLBC_NS_BEGIN


int LLBC_TaskQueue::Push(LLBC_MessageBlock *block)
{
    _msgQueue.PushBack(block);
    return LLBC_OK;
}

int LLBC_TaskQueue::Pop(LLBC_MessageBlock *&block)
{
    _msgQueue.PopFront(block);
    return LLBC_OK;
}

int LLBC_TaskQueue::PopAll(LLBC_MessageBlock *&blocks)
{
    if (_msgQueue.PopAll(blocks))
        return LLBC_OK;

    return LLBC_FAILED;
}

int LLBC_TaskQueue::TryPop(LLBC_MessageBlock *&block)
{
    if (_msgQueue.TryPopFront(block))
        return LLBC_OK;

    return LLBC_FAILED;
}

int LLBC_TaskQueue::TimedPop(LLBC_MessageBlock *&block, int interval)
{
    if (_msgQueue.TimedPopFront(block, interval))
        return LLBC_OK;

    return LLBC_FAILED;
}

size_t LLBC_TaskQueue::GetMessageSize() const
{
    return _msgQueue.GetSize();
}

void LLBC_TaskQueue::Clear()
{
    _msgQueue.Cleanup();
}


LLBC_MultiThreadTaskQueue::LLBC_MultiThreadTaskQueue()
    : _threadNum(0)
    , _processorIdGetter(nullptr)
{
}

int LLBC_MultiThreadTaskQueue::Push(int hashId, LLBC_MessageBlock *block)
{
    _msgQueues[hashId%_threadNum]->PushBack(block);
    return LLBC_OK;
}

int LLBC_MultiThreadTaskQueue::Pop(LLBC_MessageBlock *&block)
{
    _msgQueues[_processorIdGetter()]->PopFront(block);
    return LLBC_OK;
}

int LLBC_MultiThreadTaskQueue::PopAll(LLBC_MessageBlock *&blocks)
{
    if (_msgQueues[_processorIdGetter()]->PopAll(blocks))
        return LLBC_OK;

    return LLBC_FAILED;
}

int LLBC_MultiThreadTaskQueue::TryPop(LLBC_MessageBlock *&block)
{
    if (_msgQueues[_processorIdGetter()]->TryPopFront(block))
        return LLBC_OK;

    return LLBC_FAILED;
}

int LLBC_MultiThreadTaskQueue::TimedPop(LLBC_MessageBlock *&block, int interval)
{
    if (_msgQueues[_processorIdGetter()]->TimedPopFront(block, interval))
        return LLBC_OK;

    return LLBC_FAILED;
}

void LLBC_MultiThreadTaskQueue::Clear()
{
    for (auto &msgQueue : _msgQueues)
        msgQueue->Cleanup();
}

size_t LLBC_MultiThreadTaskQueue::GetMessageSize() const
{
    return _msgQueues[_processorIdGetter()]->GetSize();
}

__LLBC_NS_END
