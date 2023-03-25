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

#include "llbc/core/thread/ThreadDescriptor.h"

__LLBC_NS_BEGIN

inline LLBC_ThreadGroupDescriptor::LLBC_ThreadGroupDescriptor()
: _groupHandle(LLBC_INVALID_HANDLE)
{
}

inline LLBC_Handle LLBC_ThreadGroupDescriptor::GetGroupHandle() const
{
    return _groupHandle;
}

inline void LLBC_ThreadGroupDescriptor::SetGroupHandle(LLBC_Handle groupHandle)
{
    _groupHandle = groupHandle;
}

inline int LLBC_ThreadGroupDescriptor::AddToGroup(LLBC_ThreadDescriptor *threadDesc)
{
    if (IsInGroup(threadDesc->GetHandle()))
    {
        LLBC_SetLastError(LLBC_ERROR_REPEAT);
        return LLBC_FAILED;
    }

    threadDesc->SetGroupHandle(_groupHandle);
    _hldr2Threads.emplace(threadDesc->GetHandle(), threadDesc);

    return LLBC_OK;
}

inline bool LLBC_ThreadGroupDescriptor::IsInGroup(LLBC_Handle threadHandle) const
{
    return _hldr2Threads.find(threadHandle) != _hldr2Threads.end();
}

inline int LLBC_ThreadGroupDescriptor::RemoveFromGroup(LLBC_Handle threadHandle)
{
    const auto it = _hldr2Threads.find(threadHandle);
    if (it == _hldr2Threads.end())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _hldr2Threads.erase(it);

    return LLBC_OK;
}

inline size_t LLBC_ThreadGroupDescriptor::GetThreadsCount() const
{
    return _hldr2Threads.size();
}

inline LLBC_ThreadDescriptor *LLBC_ThreadGroupDescriptor::FindThreadDescriptor(LLBC_Handle threadHandle) const
{
    const auto it = _hldr2Threads.find(threadHandle);
    return it != _hldr2Threads.end() ? it->second : nullptr;
}

inline
const std::unordered_map<LLBC_Handle, LLBC_ThreadDescriptor *> &
LLBC_ThreadGroupDescriptor::GetAllThreadDescriptors() const
{
    return _hldr2Threads;
}

inline void LLBC_ThreadGroupDescriptor::CollectAllThreadHandles(std::vector<LLBC_Handle> &threadHandles) const
{
    threadHandles.reserve(threadHandles.size() + _hldr2Threads.size());
    LLBC_Foreach(_hldr2Threads, threadHandles.emplace_back(item.first));
}

__LLBC_NS_END
