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

class LLBC_Variant;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief LLBC library variant data type default trait class encapsulation.
 */
class LLBC_EXPORT LLBC_VariantTraits
{
public:
    /**
     * Assign operator.
     */
    static void assign(LLBC_Variant &left, const LLBC_Variant &right);

    /**
     * Relational operators.
     * ==, !=, <, >, <=, >=
     */
    static bool eq(const LLBC_Variant &left, const LLBC_Variant &right);
    static bool ne(const LLBC_Variant &left, const LLBC_Variant &right);
    static bool lt(const LLBC_Variant &left, const LLBC_Variant &right);
    static bool gt(const LLBC_Variant &left, const LLBC_Variant &right);
    static bool le(const LLBC_Variant &left, const LLBC_Variant &right);
    static bool ge(const LLBC_Variant &left, const LLBC_Variant &right);

    /**
     * Arithmetic operators.
     * +, -, *, /
     */
    static LLBC_Variant add(const LLBC_Variant &left, const LLBC_Variant &right);
    static LLBC_Variant sub(const LLBC_Variant &left, const LLBC_Variant &right);
    static LLBC_Variant mul(const LLBC_Variant &left, const LLBC_Variant &right);
    static LLBC_Variant div(const LLBC_Variant &left, const LLBC_Variant &right);

    /**
     * Arithmetic operators.
     * +=, -=, *=, /=
     */
    static void add_equal(LLBC_Variant &left, const LLBC_Variant &right);
    static void sub_equal(LLBC_Variant &left, const LLBC_Variant &right);
    static void mul_equal(LLBC_Variant &left, const LLBC_Variant &right);
    static void div_equal(LLBC_Variant &left, const LLBC_Variant &right);
};

__LLBC_NS_END
