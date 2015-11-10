/**
 * @file    TlsImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/10
 * @version 1.0
 *
 * @brief
 */

#ifdef __LLBC_CORE_THREAD_TLS_H__

#include "llbc/core/os/OS_Thread.h"

__LLBC_NS_BEGIN

template <typename ValueType>
LLBC_Tls<ValueType>::LLBC_Tls()
{
    LLBC_TlsAlloc(&_handle);
}

template <typename ValueType>
LLBC_Tls<ValueType>::~LLBC_Tls()
{
    LLBC_TlsFree(_handle);
}

template <typename ValueType>
ValueType *LLBC_Tls<ValueType>::operator ->()
{
    return this->GetValue();
}

template <typename ValueType>
const ValueType *LLBC_Tls<ValueType>::operator ->() const
{
    return this->GetValue();
}

template <typename ValueType>
ValueType &LLBC_Tls<ValueType>::operator *()
{
    return *(this->GetValue());
}

template <typename ValueType>
const ValueType &LLBC_Tls<ValueType>::operator *() const
{
    return *(this->GetValue());
}

template <typename ValueType>
ValueType *LLBC_Tls<ValueType>::GetValue()
{
    return reinterpret_cast<ValueType *>(LLBC_TlsGetValue(_handle));
}

template <typename ValueType>
const ValueType *LLBC_Tls<ValueType>::GetValue() const
{
    return reinterpret_cast<ValueType *>(LLBC_TlsGetValue(_handle));
}

template <typename ValueType>
void LLBC_Tls<ValueType>::SetValue(ValueType *value)
{
    delete reinterpret_cast<ValueType *>(LLBC_TlsGetValue(_handle));
    LLBC_TlsSetValue(_handle, value);
}

__LLBC_NS_END

#endif // __LLBC_CORE_THREAD_TLS_H__
