/**
 * @file    AutoReleasePool.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/01/19
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

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
        return LLBC_RTN_FAILED;
    }

    if (UNLIKELY(o->_autoRef >= o->_ref))
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_RTN_FAILED;
    }

    _arr->PushBack(o);
    ++o->_autoRef;
    o->Release();

    return LLBC_RTN_OK;
}

int LLBC_AutoReleasePool::RemoveObject(LLBC_Object *o)
{
    if (UNLIKELY(!o))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }
    if (o->_autoRef == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_RTN_FAILED;
    }

    _arr->Erase(o, false);
    o->_autoRef = 0;

    return LLBC_RTN_OK;
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
    return LLBC_RTN_FAILED;
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

#include "llbc/common/AfterIncl.h"
