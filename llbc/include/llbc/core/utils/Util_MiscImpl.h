/**
 * @file    Util_MiscImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/05
 * @version 1.0
 *
 * @brief
 */

#ifdef __LLBC_CORE_UTILS_UTIL_MISC_H__

#include "llbc/core/utils/Util_DelegateImpl.h"

__LLBC_NS_BEGIN

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

#endif // __LLBC_CORE_UTILS_UTIL_MISC_H__
