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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief Thread local storage value clear method enumeration, use in SetValue()/ClearValue() method.
 */
class LLBC_TlsValueClearMeth
{
public:
    enum
    {
        Begin,

        Delete = Begin,
        Free,

        Default = Delete,

        End,
    };
};

/**
 * \brief Thread local storage class encapsulation, it allow each thread
 *        allocate locations in which to store thread-specific data.
 */
template <typename ValueType>
class LLBC_Tls
{
public:
    LLBC_Tls();
    ~LLBC_Tls();

public:
    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    ValueType *operator ->();

    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    const ValueType *operator ->() const;

    /**
     * Get value associated with the tls.
     * @return ValueType & - value reference.
     */
    ValueType &operator *();

    /**
     * Get value associated with the tls.
     * @return const ValueType & - const value reference.
     */
    const ValueType &operator *() const;

    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    ValueType *GetValue();

    /**
     * Get value associated with the tls.
     * @return ValueType * - value pointer.
     */
    const ValueType *GetValue() const;

    /**
     * Set value associated with the tls.
     * @param[in] value          - value pointer.
     * @param[in] valueClearMeth - the value clear method.
     */
    void SetValue(ValueType *value, int valueClearMeth = LLBC_TlsValueClearMeth::Default);

    /**
     * Clear value associated with the tls.
     * @param[in] valueClearMeth - the value clear method.
     */
    void ClearValue(int valueClearMeth = LLBC_TlsValueClearMeth::Default);

    LLBC_DISABLE_ASSIGNMENT(LLBC_Tls);

private:
    LLBC_TlsHandle _handle;
};

__LLBC_NS_END

#include "llbc/core/thread/TlsInl.h"


