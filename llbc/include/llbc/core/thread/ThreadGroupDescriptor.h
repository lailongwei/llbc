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

/**
 * Pre-declare LLBC_ThreadDescriptor.
 */
class LLBC_ThreadDescriptor;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The thread group descriptor encapsulation.
 */
class LLBC_ThreadGroupDescriptor
{
public:
    LLBC_ThreadGroupDescriptor();
    ~LLBC_ThreadGroupDescriptor() = default;

public:
    /**
     * Get the thread group handle.
     * @return LLBC_Handle - thread handle.
     */
    LLBC_Handle GetGroupHandle() const;

    /**
     * Set the thread group groupHandle.
     * @param[in] groupHandle - thread group handle..
     */
    void SetGroupHandle(LLBC_Handle groupHandle);

public:
    /**
     * Add thread descriptor to group.
     * @param[in] threadDesc - thread descriptor.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddToGroup(LLBC_ThreadDescriptor *threadDesc);

    /**
     * Check given thread handle's thread descriptor in group.
     * @param[in] threadHandle - thread handle.
     * @return bool - return true if in group, otherwise not in group.
     */
    bool IsInGroup(LLBC_Handle threadHandle) const;

    /**
     * Remove thread descriptor from group.
     * @param[in] threadHandle handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveFromGroup(LLBC_Handle threadHandle);

    /**
     * Get this task group threads count.
     * @return size_t - thread count.
     */
    size_t GetThreadsCount() const;

public:
    /**
     * Find the specific thread descriptor by thread handle.
     * @param[in] threadHandle - thread handle.
     * @return LLBC_ThreadDescriptor * - the thread descriptor.
     */
    LLBC_ThreadDescriptor *FindThreadDescriptor(LLBC_Handle threadHandle) const;

    /**
     * Get all thread descriptors.
     * @return const std::unordered_map<LLBC_Handle, LLBC_ThreadDescriptor *> & - the thread descriptors set.
     */
    const std::unordered_map<LLBC_Handle, LLBC_ThreadDescriptor *> &
    GetAllThreadDescriptors() const;

    /**
     * Collect all thread handles.
     * @param[in] threadHandles - the thread ahandles list.
     */
    void CollectAllThreadHandles(std::vector<LLBC_Handle> &threadHandles) const;

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_ThreadGroupDescriptor);

private:
    LLBC_Handle _groupHandle;
    std::unordered_map<LLBC_Handle, LLBC_ThreadDescriptor *> _hldr2Threads;
};

__LLBC_NS_END

#include "llbc/core/thread/ThreadGroupDescriptorInl.h"
