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
