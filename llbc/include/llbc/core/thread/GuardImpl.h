/**
 * @file    GuardImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/02
 * @version 1.0
 * 
 * @brief
 */
#ifdef __LLBC_CORE_THREAD_GUARD_H__

#include "llbc/core/utils/Util_DelegateImpl.h"

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
#include "llbc/core/os/OS_Console.h"
#include "llbc/core/utils/Util_Debug.h"
#endif // Guard debug

__LLBC_NS_BEGIN

template <typename T>
inline LLBC_DeleteGuard<T>::LLBC_DeleteGuard(T *&ptr, bool setNullAfterDelete)
: _ptr(ptr)
, _setNullAfterDelete(setNullAfterDelete)
{
}

template <typename T>
inline LLBC_DeleteGuard<T>::~LLBC_DeleteGuard()
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeleteGuard: ptr[%p] will delete", _ptr);
#endif // Guard debug

    if (LIKELY(_setNullAfterDelete))
        LLBC_XDelete(_ptr);
    else
        LLBC_Delete(_ptr);

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeleteGuard: ptr[%p] deleted", _ptr);
#endif // Guard debug
}

template <typename T>
inline LLBC_DeletesGuard<T>::LLBC_DeletesGuard(T *&ptr, bool setNullAfterDeletes)
: _ptr(ptr)
, _setNullAfterDeletes(setNullAfterDeletes)
{
}

template <typename T>
inline LLBC_DeletesGuard<T>::~LLBC_DeletesGuard()
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeletesGuard: ptr array[%p] will delete", _ptr);
#endif // Guard debug

    if (LIKELY(_setNullAfterDeletes))
        LLBC_XDeletes(_ptr);
    else
        LLBC_Deletes(_ptr);

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_DeletesGuard: ptr array[%p] deleted", _ptr);
#endif // Guard debug
}

template <typename T>
inline LLBC_FreeGuard<T>::LLBC_FreeGuard(T *&ptr, bool setNullAfterFree)
: _ptr(ptr)
, _setNullAfterFree(setNullAfterFree)
{
}

template <typename T>
inline LLBC_FreeGuard<T>::~LLBC_FreeGuard()
{
#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_FreeGuard: ptr[%p] will free", _ptr);
#endif // Guard debug

    if (LIKELY(_setNullAfterFree))
        LLBC_XFree(_ptr);
    else
        LLBC_Free(_ptr);

#if LLBC_DEBUG && LLBC_CFG_THREAD_GUARD_DEBUG
    traceline("LLBC_FreeGuard: ptr[%p] freed", _ptr);
#endif // Guard debug
}

inline LLBC_InvokeGuard::LLBC_InvokeGuard(LLBC_GuardFunc func, void *data)
: _func(func)
, _meth(NULL)

, _data(data)
{
}

template <typename Object>
inline LLBC_InvokeGuard::LLBC_InvokeGuard(Object *obj, void (Object::*meth)(void *), void *data)
: _func(NULL)
, _meth(new LLBC_Delegate1<void, Object, void *>(obj, meth))

, _data(data)
{
}

inline LLBC_InvokeGuard::~LLBC_InvokeGuard()
{
    if (_func)
    {
        (*_func)(_data);
    }
    else
    {
        _meth->Invoke(_data);
        LLBC_Delete(_meth);
    }
}
__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_GUARD_H__
