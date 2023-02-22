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
 * \brief LLBC Variant arithmetic util class encpasulation.
 *        just support bool/[su]int8/[su]int16/[su]int32/[su]int64/float/double type variant object.
 */
class LLBC_EXPORT LLBC_VariantArithmetic
{
public:
    /**
     * Operations type enumeration.
     */
    enum
    {
        VT_ARITHMETIC_ADD = 0,
        VT_ARITHMETIC_SUB = 1,
        VT_ARITHMETIC_MUL = 2,
        VT_ARITHMETIC_DIV = 3,

        VT_ARITHMETIC_END
    };

    /**
     * Performs arithmetic.
     * @param[in/out] left - left object.
     * @param[in] right    - right object.
     * @param[in] type     - operation type, see enumerations value.
     */
    static void Performs(LLBC_Variant &left, const LLBC_Variant &right, int type);

private:
    /**
     * Implementation functions.
     */
    static void Performs_bool_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_int8_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_uint8_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_int16_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_uint16_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_int32_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_uint32_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_long_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_ulong_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_ptr_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_int64_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_uint64_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_float_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_double_any(LLBC_Variant &left, const LLBC_Variant &right, int type);

    /**
     * Implementation function.
     */
    template <typename RawType>
    static RawType Performs_raw_operation(RawType left, RawType right, int type);
};

__LLBC_NS_END

#include "llbc/core/variant/VariantArithmeticInl.h"



 