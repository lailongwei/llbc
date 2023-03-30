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

#include "llbc/core/entity/IHolder.h"

__LLBC_NS_BEGIN

/**
 * \brief The holder class encapsulation.
 */
template <typename ValueType>
class LLBC_Holder : public LLBC_IHolder
{
public:
    explicit LLBC_Holder(ValueType *value);
    virtual ~LLBC_Holder();

public:
    /**
     * Get holder value.
     * @return [in] void * - value.
     */
    virtual void *GetValue() const;

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Holder);

private:
    ValueType *_value;
};

__LLBC_NS_END

#include "llbc/core/entity/HolderInl.h"


