/**
 * @file    SingletonImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/06
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_CORE_SINGLETON_SINGLETON_H__

#include "llbc/core/thread/Tls.h"

__LLBC_NS_BEGIN

template <typename T, typename LockType, bool ThreadUnit>
LockType LLBC_Singleton<T, LockType, ThreadUnit>::_lock;

template <typename T, typename LockType, bool ThreadUnit>
T *LLBC_Singleton<T, LockType, ThreadUnit>::_instance;

template <typename T, typename LockType, bool ThreadUnit>
LLBC_Tls<T> *LLBC_Singleton<T, LockType, ThreadUnit>::_tls;

template <typename T, typename LockType, bool ThreadUnit>
T *LLBC_Singleton<T, LockType, ThreadUnit>::Instance()
{
    if (UNLIKELY(ThreadUnit))
    {
        if (UNLIKELY(!_tls))
        {
            _lock.Lock();
            if (!_tls)
                _tls = new LLBC_Tls<T>;

            _lock.Unlock();
        }

        if (LIKELY(_tls->GetValue()))
            return _tls->GetValue();

        _tls->SetValue(new T);

        return _tls->GetValue();
    }

    if (UNLIKELY(!_instance))
    {
        _lock.Lock();
        if (!_instance)
            _instance = new T;

        _lock.Unlock();
    }

    return _instance;
}

template <typename T, typename LockType, bool ThreadUnit>
void LLBC_Singleton<T, LockType, ThreadUnit>::Release()
{
    if (UNLIKELY(ThreadUnit))
    {
        if (!_tls)
            return;

        _lock.Lock();
        LLBC_XDelete(_tls);
        _lock.Unlock();

        return;
    }

    if (!_instance)
        return;

    _lock.Lock();
    LLBC_XDelete(_instance);
    _lock.Unlock();
}

__LLBC_NS_END

#endif // __LLBC_CORE_SINGLETON_SINGLETON_H__
