/**
 * @file    VariantTraits.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/11
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_VARIANT_VARIANT_TRAITS_H__
#define __LLBC_CORE_VARIANT_VARIANT_TRAITS_H__

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

#endif // !__LLBC_CORE_VARIANT_VARIANT_TRAITS_H__
