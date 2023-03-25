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

__LLBC_NS_BEGIN

inline LLBC_ThreadDescriptor::LLBC_ThreadDescriptor()
: _threadHandle(LLBC_INVALID_HANDLE)
, _nativeHandle(LLBC_INVALID_NATIVE_THREAD_HANDLE)
, _groupHandle(LLBC_INVALID_HANDLE)

, _priority(LLBC_ThreadPriority::Normal)
, _state(LLBC_ThreadState::Stoped)
{
}

inline LLBC_Handle LLBC_ThreadDescriptor::GetHandle() const
{
    return _threadHandle;
}

inline void LLBC_ThreadDescriptor::SetHandle(LLBC_Handle handle)
{
    _threadHandle = handle;
}

inline LLBC_NativeThreadHandle LLBC_ThreadDescriptor::GetNativeHandle() const
{
    return _nativeHandle;
}

inline void LLBC_ThreadDescriptor::SetNativeHandle(LLBC_NativeThreadHandle handle)
{
    _nativeHandle = handle;
}

inline LLBC_Handle LLBC_ThreadDescriptor::GetGroupHandle() const
{
    return _groupHandle;
}

inline void LLBC_ThreadDescriptor::SetGroupHandle(LLBC_Handle handle)
{
    _groupHandle = handle;
}

inline int LLBC_ThreadDescriptor::GetPriority() const
{
    return _priority;
}

inline void LLBC_ThreadDescriptor::SetPriority(int priority)
{
    _priority = priority;
}

inline int LLBC_ThreadDescriptor::GetState() const
{
    return _state;
}

inline void LLBC_ThreadDescriptor::SetState(int state)
{
    _state = state;
}

__LLBC_NS_END
