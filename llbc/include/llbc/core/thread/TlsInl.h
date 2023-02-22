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
void LLBC_Tls<ValueType>::SetValue(ValueType *value, int valueClearMeth)
{
    void *oldValue = LLBC_TlsGetValue(_handle);
    if (UNLIKELY(oldValue == value))
        return;

    if (UNLIKELY(oldValue))
    {
        switch (valueClearMeth)
        {
        case LLBC_TlsValueClearMeth::Free:
            free(oldValue);
            break;

        default: // Default using delete to clear value.
            delete reinterpret_cast<ValueType *>(oldValue);
            break;
        }
    }

    LLBC_TlsSetValue(_handle, value);
}

template <typename ValueType>
void LLBC_Tls<ValueType>::ClearValue(int valueClearMeth)
{
    this->SetValue(nullptr, valueClearMeth);
}

__LLBC_NS_END
