/**
 * @file    ThreadGroupDescriptor.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_THREAD_GROUP_DESC_H__
#define __LLBC_CORE_THREAD_THREAD_GROUP_DESC_H__

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
    ~LLBC_ThreadGroupDescriptor();

public:
    /**
     * Get the thread group handle.
     * @return LLBC_Handle - thread handle.
     */
    LLBC_Handle GetGroupHandle() const;

    /**
     * Set the thread group handle.
     * @param[in] handle - group handle.
     */
    void SetGroupHandle(LLBC_Handle handle);

    /**
     * Get the next thread group descriptor.
     * @return LLBC_ThreadGroupDescriptor * - next thread group descriptor.
     */
    LLBC_ThreadGroupDescriptor *GetGroupNext() const;

    /**
     * Set the next thread group descriptor.
     * @param[in] next - the next thread group descriptor.
     */
    void SetGroupNext(LLBC_ThreadGroupDescriptor *next);

public:
    /**
     * Add thread descriptor to group.
     * @param[in] desc - thread descriptor.
     * @return int - return 0 if success, otherwise return -1.
     */
    int AddToGroup(LLBC_ThreadDescriptor *desc);

    /**
     * Check given thread handle's thread descriptor in group.
     * @param[in] bool - thread handle.
     * @return bool - return true if in group, otherwise not in group.
     */
    bool IsInGroup(LLBC_Handle threadHandle) const;

    /**
     * Remove thread descriptor from group.
     * @param[in] thread handle.
     * @return int - return 0 if success, otherwise return -1.
     */
    int RemoveFromGroup(LLBC_Handle threadHandle);

    /**
     * Get this task group thread count.
     * @return uint32 - thread count.
     */
    uint32 GetThreadCount() const;

public:
    /**
     * Find the specific thread descriptor by thread handle.
     * @param[in] threadHandle - thread handle.
     * @return LLBC_ThreadDescriptor * - the thread descriptor.
     */
    LLBC_ThreadDescriptor *FindThreadDescriptor(LLBC_Handle threadHandle) const;

    /**
     * Find the group's first thread descriptor.
     * @return LLBC_ThreadDescriptor * - the first thread descriptor.
     */
    LLBC_ThreadDescriptor *FindFirstThreadDescriptor() const;

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_ThreadGroupDescriptor);

private:
    LLBC_Handle _groupHandle;
    LLBC_ThreadGroupDescriptor *_nextGroup;

    LLBC_ThreadDescriptor *_firstThreadDesc;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_THREAD_GROUP_DESC_H__
