/**
 * @file    ThreadDescriptor.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/13
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/os/OS_Thread.h"
#include "llbc/core/thread/ThreadDescriptor.h"

__LLBC_NS_BEGIN

LLBC_ThreadDescriptor::LLBC_ThreadDescriptor()
{
    _threadHandle = LLBC_INVALID_HANDLE;
    _nativeHandle = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    _groupHandle = LLBC_INVALID_HANDLE;

    _stackSize = LLBC_CFG_THREAD_DFT_STACK_SIZE;
    _priority = LLBC_ThreadPriority::Normal;
    _state = LLBC_ThreadState::Stoped;
    _flags = LLBC_ThreadFlag::Joinable;

    _task = NULL;
    _nextThread = NULL;
    _groupNextThread = NULL;
}

LLBC_ThreadDescriptor::~LLBC_ThreadDescriptor()
{
}

LLBC_Handle LLBC_ThreadDescriptor::GetHandle() const
{
    return _threadHandle;
}

void LLBC_ThreadDescriptor::SetHandle(LLBC_Handle handle)
{
    _threadHandle = handle;
}

LLBC_NativeThreadHandle LLBC_ThreadDescriptor::GetNativeHandle() const
{
    return _nativeHandle;
}

void LLBC_ThreadDescriptor::SetNativeHandle(LLBC_NativeThreadHandle handle)
{
    _nativeHandle = handle;
}

LLBC_Handle LLBC_ThreadDescriptor::GetGroupHandle() const
{
    return _groupHandle;
}

void LLBC_ThreadDescriptor::SetGroupHandle(LLBC_Handle handle)
{
    _groupHandle = handle;
}

int LLBC_ThreadDescriptor::GetStackSize() const
{
    return _stackSize;
}

void LLBC_ThreadDescriptor::SetStackSize(int size)
{
    _stackSize = size;
}

int LLBC_ThreadDescriptor::GetPriority() const
{
    return _priority;
}

void LLBC_ThreadDescriptor::SetPriority(int priority)
{
    _priority = priority;
}

int LLBC_ThreadDescriptor::GetState() const
{
    return _state;
}

void LLBC_ThreadDescriptor::SetState(int state)
{
    _state = state;
}

int LLBC_ThreadDescriptor::GetFlags() const
{
    return _flags;
}

void LLBC_ThreadDescriptor::SetFlags(int flags)
{
    _flags = flags;
}

LLBC_BaseTask *LLBC_ThreadDescriptor::GetTask() const
{
    return _task;
}

void LLBC_ThreadDescriptor:: SetTask(LLBC_BaseTask *task)
{
    _task = task;
}

LLBC_ThreadDescriptor *LLBC_ThreadDescriptor::GetThreadNext() const
{
    return _nextThread;
}

void LLBC_ThreadDescriptor::SetThreadNext(LLBC_ThreadDescriptor *desc)
{
    _nextThread = desc;
}

LLBC_ThreadDescriptor *LLBC_ThreadDescriptor::GetGroupThreadNext() const
{
    return _groupNextThread;
}

void LLBC_ThreadDescriptor::SetGroupThreadNext(LLBC_ThreadDescriptor *desc)
{
    _groupNextThread = desc;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
