/**
 * @file    VariantArithmetic.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/17
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_VARIANT_VARIANT_ARITHMETIC_H__
#define __LLBC_CORE_VARIANT_VARIANT_ARITHMETIC_H__

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
    static void Performs_int64_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_uint64_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_float_any(LLBC_Variant &left, const LLBC_Variant &right, int type);
    static void Performs_double_any(LLBC_Variant &left, const LLBC_Variant &right, int type);

    /**
     * Implementation function.
     */
    template <typename RawType>
    static RawType Performs_raw_operation(RawType left, RawType right, int type)
    {
        switch(type)
        {
        case VT_ARITHMETIC_ADD:
            return left + right;

        case VT_ARITHMETIC_SUB:
            return left - right;

        case VT_ARITHMETIC_MUL:
            return left * right;

        case VT_ARITHMETIC_DIV:
            return left / right;

        default:
            break;
        }

        return RawType();
    }
};

__LLBC_NS_END

#endif // !__LLBC_CORE_VARIANT_VARIANT_ARITHMETIC_H__

 