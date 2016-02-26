/**
 * @file    AutoReleasePoolStack.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/20
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/objbase/Object.h"
#include "llbc/objbase/AutoReleasePool.h"
#include "llbc/objbase/AutoReleasePoolStack.h"

__LLBC_NS_BEGIN

LLBC_AutoReleasePoolStack::LLBC_AutoReleasePoolStack()
: _head(NULL)
{
}

LLBC_AutoReleasePoolStack::~LLBC_AutoReleasePoolStack()
{
    typedef LLBC_AutoReleasePool _Pool;
    
    std::vector<_Pool *> pools;
    for (_Pool *pool = _head; pool != NULL; pool = pool->GetPoolNext())
    {
        pools.push_back(pool);
    }

    for (size_t i = 0; i < pools.size(); i++)
    {
        delete pools[i];
    }
}

int LLBC_AutoReleasePoolStack::AddObject(LLBC_Object *o)
{
    return _head->AddObject(o);
}

int LLBC_AutoReleasePoolStack::RemoveObject(LLBC_Object *o)
{
    typedef LLBC_AutoReleasePool _Pool;

    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    _Pool *pool = _head;
    for (; pool != NULL; pool = pool->GetPoolNext())
    {
        if (pool->RemoveObject(o) == LLBC_OK)
        {
            return LLBC_OK;
        }
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);

    return LLBC_FAILED;
}

int LLBC_AutoReleasePoolStack::PushPool(LLBC_AutoReleasePool *pool)
{
    if (UNLIKELY(!pool))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    pool->SetPoolNext(_head);
    _head = pool;

    return LLBC_OK;
}

int LLBC_AutoReleasePoolStack::PopPool(LLBC_AutoReleasePool *pool)
{
    typedef LLBC_AutoReleasePool _Pool;

    if (UNLIKELY(!pool))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (!_head)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    if (_head == pool)
    {
        _head = _head->GetPoolNext();
        return LLBC_OK;
    }

    _Pool *curPool = _head;
    while (curPool->GetPoolNext())
    {
        _Pool *temp = curPool->GetPoolNext();
        if (temp == pool)
        {
            curPool->SetPoolNext(curPool->GetPoolNext()->GetPoolNext());
            return LLBC_OK;
        }

        curPool = temp;
    }

    LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
    return LLBC_FAILED;
}

LLBC_AutoReleasePool *LLBC_AutoReleasePoolStack::GetCurrentReleasePool()
{
    return _head;
}

void LLBC_AutoReleasePoolStack::Purge()
{
    typedef LLBC_AutoReleasePool _Pool;

    _Pool *pool = _head;
    while (pool)
    {
        pool->Purge();
        pool = pool->GetPoolNext();
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
