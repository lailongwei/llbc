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

#pragma once

#include "llbc/core/os/OS_Atomic.h"

__LLBC_NS_BEGIN

inline const char *LLBC_TaskState::GetDesc(int taskState)
{
    LLBC_ReturnIf(taskState == NotActivated, "NotActivated");
    LLBC_ReturnIf(taskState == Activating, "Activating");
    LLBC_ReturnIf(taskState == Activated, "Activated");
    LLBC_ReturnIf(taskState == Deactivating, "Deactivating");

    return "UnknownTaskState";
}

inline bool LLBC_Task::IsActivated() const
{
    return _taskState == LLBC_TaskState::Activated;
}

inline int LLBC_Task::GetTaskState() const
{
    return _taskState;
}

inline LLBC_ThreadMgr *LLBC_Task::GetThreadMgr() const
{
    return _threadMgr;
}

inline LLBC_Handle LLBC_Task::GetThreadGroupHandle() const
{
    return _threadGroupHandle;
}

inline int LLBC_Task::Push(LLBC_MessageBlock *block)
{
    _msgQueue.PushBack(block);
    return LLBC_OK;
}

inline int LLBC_Task::Pop(LLBC_MessageBlock *&block)
{
    _msgQueue.PopFront(block);
    return LLBC_OK;
}

inline int LLBC_Task::PopAll(LLBC_MessageBlock *&blocks)
{
    if (_msgQueue.PopAll(blocks))
        return LLBC_OK;

    return LLBC_FAILED;
}

inline int LLBC_Task::TryPop(LLBC_MessageBlock *&block)
{
    if (_msgQueue.TryPopFront(block))
        return LLBC_OK;

    return LLBC_FAILED;
}

inline int LLBC_Task::TimedPop(LLBC_MessageBlock *&block, int interval)
{
    if (_msgQueue.TimedPopFront(block, interval))
        return LLBC_OK;

    return LLBC_FAILED;
}

inline size_t LLBC_Task::GetMessageSize() const
{
    return _msgQueue.GetSize();
}

__LLBC_NS_END
