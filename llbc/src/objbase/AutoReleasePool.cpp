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
#include "llbc/common/BeforeIncl.h"

#include "llbc/common/Config.h"

#if LLBC_CFG_OBJBASE_ENABLED

#include "llbc/objbase/Object.h"
#include "llbc/objbase/Array.h"
#include "llbc/objbase/AutoReleasePool.h"
#include "llbc/objbase/AutoReleasePoolStack.h"

__LLBC_NS_BEGIN

LLBC_AutoReleasePool::LLBC_AutoReleasePool()
: _arr(NULL)
, _next(NULL)
{
    _arr = new LLBC_Array;
    typedef LLBC_AutoReleasePoolStack _Stack;

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    _Stack *stack = reinterpret_cast<_Stack *>(tls->objbaseTls.poolStack);

    stack->PushPool(this);
}

LLBC_AutoReleasePool::~LLBC_AutoReleasePool()
{
    typedef LLBC_AutoReleasePoolStack _Stack;

    __LLBC_LibTls *tls = __LLBC_GetLibTls();
    _Stack *stack = reinterpret_cast<_Stack *>(tls->objbaseTls.poolStack);

    // Popup up from pool stack.
    stack->PopPool(this);

    // To make more efficient, set array all elements' _poolStack to null.
    LLBC_Array::Iter it = _arr->Begin();
    for (; it != _arr->End(); it++)
    {
        (*it)->_poolStack = NULL;
    }

    // Release array.
    _arr->Release();
}

int LLBC_AutoReleasePool::AddObject(LLBC_Object *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }

    if (UNLIKELY(o->_autoRef >= o->_ref))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _arr->PushBack(o);
    ++o->_autoRef;
    o->Release();

    return LLBC_OK;
}

int LLBC_AutoReleasePool::RemoveObject(LLBC_Object *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_FAILED;
    }
    if (o->_autoRef == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    _arr->Erase(o, false);
    o->_autoRef = 0;

    return LLBC_OK;
}

void LLBC_AutoReleasePool::Purge()
{
    LLBC_Array::Iter iter = _arr->Begin();
    for (; iter != _arr->End(); iter++)
    {
        LLBC_Object *obj = *iter;
        --obj->_autoRef;
    }

    _arr->Clear();
}

int LLBC_AutoReleasePool::AutoRelease()
{
    LLBC_SetLastError(LLBC_ERROR_PERM);
    return LLBC_FAILED;
}

LLBC_Object *LLBC_AutoReleasePool::Clone() const
{
    LLBC_SetLastError(LLBC_ERROR_PERM);
    return NULL;
}

LLBC_AutoReleasePool *LLBC_AutoReleasePool::GetPoolNext()
{
    return _next;
}

void LLBC_AutoReleasePool::SetPoolNext(LLBC_AutoReleasePool *next)
{
    _next = next;
}

__LLBC_NS_END

#endif // LLBC_CFG_OBJBASE_ENABLED

#include "llbc/common/AfterIncl.h"
