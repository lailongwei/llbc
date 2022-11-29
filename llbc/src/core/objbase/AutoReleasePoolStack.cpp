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


#include "llbc/common/Export.h"

#include "llbc/common/Config.h"

#include "llbc/core/objbase/Object.h"
#include "llbc/core/objbase/AutoReleasePool.h"
#include "llbc/core/objbase/AutoReleasePoolStack.h"

__LLBC_NS_BEGIN

LLBC_AutoReleasePoolStack::LLBC_AutoReleasePoolStack()
: _head(nullptr)
{
}

LLBC_AutoReleasePoolStack::~LLBC_AutoReleasePoolStack()
{
    typedef LLBC_AutoReleasePool _Pool;
    
    std::vector<_Pool *> pools;
    for (_Pool *pool = _head; pool != nullptr; pool = pool->GetPoolNext())
        pools.push_back(pool);

    for (size_t i = 0; i < pools.size(); ++i)
        delete pools[i];
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
    for (; pool != nullptr; pool = pool->GetPoolNext())
    {
        if (pool->RemoveObject(o) == LLBC_OK)
            return LLBC_OK;
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

LLBC_AutoReleasePoolStack::_This * LLBC_AutoReleasePoolStack::GetCurrentThreadReleasePoolStack()
{
    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    return reinterpret_cast<_This *>(tls->objbaseTls.poolStack);
}

__LLBC_NS_END
