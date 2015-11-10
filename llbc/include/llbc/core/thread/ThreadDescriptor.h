/**
 * @file    ThreadDescriptor.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_THREAD_THREAD_DESC_H__
#define __LLBC_CORE_THREAD_THREAD_DESC_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN
class LLBC_BaseTask;
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Thread descriptor class encapsulation, using by thread manager internal.
 */
class LLBC_ThreadDescriptor
{
public:
    LLBC_ThreadDescriptor();
    ~LLBC_ThreadDescriptor();

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
     * Get thread stack size.
     * @return int - stack size.
     */
    int GetStackSize() const;

    /**
     * Set thread stack size.
     * @param[in] size - stack size.
     */
    void SetStackSize(int size);

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

    /**
     * Get thread flags.
     * @return int - thread flags.
     */
    int GetFlags() const;

    /**
     * Set thread flags.
     * @param[in] flags - thread flags.
     */
    void SetFlags(int flags);

public:
    /**
     * Get task.
     * @return LLBC_BaseTask * - task pointer.
     */
    LLBC_BaseTask *GetTask() const;

    /**
     * Set task.
     * @param[in] task - task pointer.
     */
    void SetTask(LLBC_BaseTask *task);

public:
    /**
     * Get next thread descriptor.
     * @return LLBC_ThreadDescriptor * - next thread descriptor pointer.
     */
    LLBC_ThreadDescriptor *GetThreadNext() const;

    /**
     * Set next thread descriptor.
     * @param[in] LLBC_ThreadDescriptor * - next thread descriptor pointer.
     */
    void SetThreadNext(LLBC_ThreadDescriptor *desc);

    /**
     * Get next thread descriptor(call ThreadGroupDescriptor).
     * @return LLBC_ThreadDescriptor * - next thread descriptor pointer.
     */
    LLBC_ThreadDescriptor *GetGroupThreadNext() const;

    /**
     * Set next thread descriptor(call by ThreadGroupDescriptor).
     * @param[in] desc - thread descriptor.
     */
    void SetGroupThreadNext(LLBC_ThreadDescriptor *desc);

private:
    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_ThreadDescriptor);

private:
    LLBC_Handle _threadHandle;
    LLBC_NativeThreadHandle _nativeHandle;
    LLBC_Handle _groupHandle;

    int _stackSize;
    int _priority;
    int _state;
    int _flags;

    LLBC_BaseTask *_task;

    LLBC_ThreadDescriptor *_nextThread;
    LLBC_ThreadDescriptor *_groupNextThread;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_THREAD_THREAD_DESC_H__
