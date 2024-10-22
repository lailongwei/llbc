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

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
#include "llbc/core/os/OS_Console.h"
#include "llbc/core/utils/Util_Debug.h"
#endif // Guard debug

__LLBC_NS_BEGIN

template <typename T>
LLBC_DeleteGuard<T>::LLBC_DeleteGuard(T *&ptr, bool setNullAfterDelete)
: _ptr(ptr)
, _setNullAfterDelete(setNullAfterDelete)
{
}

template <typename T>
LLBC_DeleteGuard<T>::~LLBC_DeleteGuard()
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeleteGuard: ptr[%p] will delete", _ptr);
#endif // Guard debug

    if (LIKELY(_setNullAfterDelete))
        LLBC_XDelete(_ptr);
    else
        delete _ptr;

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeleteGuard: ptr[%p] deleted", _ptr);
#endif // Guard debug
}

template <typename T>
LLBC_DeletesGuard<T>::LLBC_DeletesGuard(T *&ptr, bool setNullAfterDeletes)
: _ptr(ptr)
, _setNullAfterDeletes(setNullAfterDeletes)
{
}

template <typename T>
LLBC_DeletesGuard<T>::~LLBC_DeletesGuard()
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeletesGuard: ptr array[%p] will delete", _ptr);
#endif // Guard debug

    if (LIKELY(_setNullAfterDeletes))
        LLBC_XDeletes(_ptr);
    else
        delete[] _ptr;

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeletesGuard: ptr array[%p] deleted", _ptr);
#endif // Guard debug
}

template <typename T>
LLBC_FreeGuard<T>::LLBC_FreeGuard(T *&ptr, bool setNullAfterFree)
: _ptr(ptr)
, _setNullAfterFree(setNullAfterFree)
{
}

template <typename T>
LLBC_FreeGuard<T>::~LLBC_FreeGuard()
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_FreeGuard: ptr[%p] will free", _ptr);
#endif // Guard debug

    if (LIKELY(_setNullAfterFree))
        LLBC_XFree(_ptr);
    else
        free(_ptr);

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_FreeGuard: ptr[%p] freed", _ptr);
#endif // Guard debug
}

template <typename Func, typename ...Args>
LLBC_InvokeGuard::LLBC_InvokeGuard(const Func &func, Args ...args)
: _deleg(std::bind(func, std::forward<Args>(args)...))
{
}

template <typename Obj, typename ...Args>
LLBC_InvokeGuard::LLBC_InvokeGuard(Obj *obj, void (Obj::*meth)(Args ...), Args ...args)
: _deleg(std::bind(meth, obj, std::forward<Args>(args)...))
{
}

inline LLBC_InvokeGuard::~LLBC_InvokeGuard()
{
    _deleg();
}

__LLBC_NS_END
