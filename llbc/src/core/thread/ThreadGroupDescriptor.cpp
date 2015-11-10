/**
 * @file    ThreadGroupDescriptor.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/16
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/thread/ThreadDescriptor.h"
#include "llbc/core/thread/ThreadGroupDescriptor.h"

__LLBC_NS_BEGIN

LLBC_ThreadGroupDescriptor::LLBC_ThreadGroupDescriptor()
    : _groupHandle(LLBC_INVALID_HANDLE)
    , _nextGroup(NULL)
    , _firstThreadDesc(NULL)
{
}

LLBC_ThreadGroupDescriptor::~LLBC_ThreadGroupDescriptor()
{
}


LLBC_Handle LLBC_ThreadGroupDescriptor::GetGroupHandle() const
{
    return _groupHandle;
}

void LLBC_ThreadGroupDescriptor::SetGroupHandle(LLBC_Handle handle)
{
    _groupHandle = handle;
}

LLBC_ThreadGroupDescriptor *LLBC_ThreadGroupDescriptor::GetGroupNext() const
{
    return _nextGroup;
}

void LLBC_ThreadGroupDescriptor::SetGroupNext(LLBC_ThreadGroupDescriptor *next)
{
    _nextGroup = next;
}

int LLBC_ThreadGroupDescriptor::AddToGroup(LLBC_ThreadDescriptor *desc)
{
    if (UNLIKELY(!desc))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (!_firstThreadDesc)
    {
        desc->SetGroupThreadNext(NULL);
        desc->SetGroupHandle(_groupHandle);

        _firstThreadDesc = desc;

        return LLBC_RTN_OK;
    }

    LLBC_ThreadDescriptor *threadDesc = _firstThreadDesc;
    while (threadDesc->GetGroupThreadNext())
    {
        LLBC_ThreadDescriptor *tempDesc = threadDesc;
        threadDesc = threadDesc->GetGroupThreadNext();
        if (tempDesc->GetHandle() == desc->GetHandle())
        {
            LLBC_SetLastError(LLBC_ERROR_EXIST);
            return LLBC_RTN_FAILED;
        }
    }

    desc->SetGroupThreadNext(NULL);
    desc->SetGroupHandle(_groupHandle);

    threadDesc->SetGroupThreadNext(desc);

    return LLBC_RTN_OK;
}

bool LLBC_ThreadGroupDescriptor::IsInGroup(LLBC_Handle threadHandle) const
{
    if (UNLIKELY(threadHandle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return false;
    }

    if (!_firstThreadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return false;
    }

    LLBC_ThreadDescriptor *threadDesc = _firstThreadDesc;
    while (threadDesc)
    {
        if (threadDesc->GetHandle() == threadHandle)
        {
            return true;
        }

        threadDesc = threadDesc->GetGroupThreadNext();
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

    return false;
}

int LLBC_ThreadGroupDescriptor::RemoveFromGroup(LLBC_Handle threadHandle)
{
    if (UNLIKELY(threadHandle == LLBC_INVALID_HANDLE))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    if (!_firstThreadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    if (_firstThreadDesc->GetHandle() == threadHandle)
    {
        _firstThreadDesc = _firstThreadDesc->GetGroupThreadNext();
        return LLBC_RTN_OK;
    }

    if (!_firstThreadDesc->GetGroupThreadNext())
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    LLBC_ThreadDescriptor *prevThreadDesc = _firstThreadDesc;
    LLBC_ThreadDescriptor *threadDesc = _firstThreadDesc->GetGroupThreadNext();
    while (threadDesc)
    {
        if (threadDesc->GetHandle() == threadHandle)
        {
            prevThreadDesc->SetGroupThreadNext(
                threadDesc->GetGroupThreadNext());

            return LLBC_RTN_OK;
        }

        prevThreadDesc = threadDesc;
        threadDesc = threadDesc->GetGroupThreadNext();
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return LLBC_RTN_FAILED;
}

uint32 LLBC_ThreadGroupDescriptor::GetThreadCount() const
{
    if (!_firstThreadDesc)
    {
        return 0;
    }

    uint32 threadCount = 0;
    LLBC_ThreadDescriptor *threadDesc = _firstThreadDesc;
    while (threadDesc)
    {
        ++ threadCount;
        threadDesc = threadDesc->GetGroupThreadNext();
    }

    return threadCount;
}

LLBC_ThreadDescriptor *LLBC_ThreadGroupDescriptor::FindThreadDescriptor(LLBC_Handle threadHandle) const
{
    if (UNLIKELY(threadHandle))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return NULL;
    }

    if (!_firstThreadDesc)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return NULL;
    }

    LLBC_ThreadDescriptor *threadDesc = _firstThreadDesc;
    while (threadDesc)
    {
        if (threadDesc->GetHandle() == threadHandle)
        {
            return threadDesc;
        }

        threadDesc = threadDesc->GetGroupThreadNext();
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return NULL;
}

LLBC_ThreadDescriptor *LLBC_ThreadGroupDescriptor::FindFirstThreadDescriptor() const
{
    return _firstThreadDesc;
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
