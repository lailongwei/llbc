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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

template <template <typename> class Task>
 void LLBC_TaskQueue::InitQueue(std::in_place_type_t<Task<LLBC_TaskQueue>>, int)
{
}

template <template <typename> class Task> 
void LLBC_MultiThreadTaskQueue::InitQueue(std::in_place_type_t<Task<LLBC_MultiThreadTaskQueue>>, int threadNum)
{
    _processorIdGetter = &Task<LLBC_MultiThreadTaskQueue>::GetProcessorId;
    _threadNum = threadNum;
    _msgQueues.reserve(_threadNum);

    for (size_t i = 0; i < _threadNum; ++i)
        _msgQueues.emplace_back(std::make_unique<LLBC_MessageQueue>());
}

__LLBC_NS_END
