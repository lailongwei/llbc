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
class LLBC_Task;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Thread descriptor class encapsulation, using by thread manager internal.
 */
class LLBC_ThreadDescriptor
{
public:
    LLBC_ThreadDescriptor();
    ~LLBC_ThreadDescriptor() = default;

public:
    /**
     * Get thread handle(generate by thread manager).
     * @return LLBC_Handle - thread handle.
     */
    LLBC_Handle GetHandle() const;

    /**
     * Set thread handle(set by thread manager).
     * @param[in] handle - thread handle.
     */
    void SetHandle(LLBC_Handle handle);

    /**
     * Get native thread handle(generate by OS).
     * @return LLBC_NativeThreadHandle - native thread handle.
     */
    LLBC_NativeThreadHandle GetNativeHandle() const;

    /**
     * Set native thread handle(set by OS).
     * @param[in] handle - native thread handle.
     */
    void SetNativeHandle(LLBC_NativeThreadHandle handle);

    /**
     * Get thread group handle(generate by thread manager).
     * @return LLBC_Handle - thread group handle.
     */
    LLBC_Handle GetGroupHandle() const;

    /**
     * Set thread group handle(generate by thread manager).
     * @param[in] handle - thread group handle.
     */
    void SetGroupHandle(LLBC_Handle handle);

    /**
     * Get thread priority.
     * @return int - thread priority.
     */
    int GetPriority() const;

    /**
     * Set thread priority.
     * @param[in] priority - thread priority.
     */
    void SetPriority(int priority);

    /**
     * Get thread state.
     * @return int - thread state.
     */
    int GetState() const;

    /**
     * Set thread state.
     * @param[in] state - thread state.
     */
    void SetState(int state);

private:
    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_ThreadDescriptor);

private:
    LLBC_Handle _threadHandle;
    LLBC_NativeThreadHandle _nativeHandle;
    LLBC_Handle _groupHandle;

    int _priority;
    int _state;
};

__LLBC_NS_END

#include "llbc/core/thread/ThreadDescriptorInl.h"
