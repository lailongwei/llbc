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

#include "llbc/common/Macro.h"
#include "llbc/common/OSHeader.h"

__LLBC_NS_BEGIN

/**
 * \brief Template specification judge function.
 */
template <typename Class, template <typename...> class ClassTemplate>
struct LLBC_IsTemplSpec : public std::false_type {};

template <template <typename...> class ClassTemplate, typename... Args>
struct LLBC_IsTemplSpec<ClassTemplate<Args...>, ClassTemplate> : public std::true_type {};

/**
 * \brief Special Template specification judge function: std::array.
 */
template <typename ArrayClass, template <typename ValueType, size_t N> class ArrayClassTemplate>
struct LLBC_IsSTLArraySpec : std::false_type {};

template <template <typename ValueType, size_t N> class ArrayClassTemplate, typename ValueType, size_t N>
struct LLBC_IsSTLArraySpec<ArrayClassTemplate<ValueType, N>, ArrayClassTemplate>
    : public std::true_type {};

/**
 * \brief Extract given T pure type.
 */
template <typename T>
struct LLBC_ExtractPureType
{
    typedef std::remove_const_t<std::remove_pointer_t< std::remove_reference_t<T>>> type;
};

/**
 * \brief Integer sequence.
 * 
 */
template <int...> struct LLBC_IntSequence {};

/**
 * \brief Make integer sequence.
 * 
 */
template <int N, int... Is> struct LLBC_MakeIntSequence :
    LLBC_MakeIntSequence<N - 1, N - 1, Is...> {};
template <int... Is> struct LLBC_MakeIntSequence<0, Is...> :
    LLBC_IntSequence<Is...> {};

/**
 * \brief LLBC library specific placeholder template.
 * 
 */
template <int>
struct LLBC_PlaceHolderTemplate {};

__LLBC_NS_END

namespace std
{

/**
 * \brief Specification std::is_placeholder<LLBC_PlaceHolderTemplate<N> >.
 * 
 */
template <int N>
struct is_placeholder<LLBC_NS LLBC_PlaceHolderTemplate<N> > :
    integral_constant<int, N + 1> {};

}

__LLBC_INTERNAL_NS_BEGIN

/**
 * \brief Class method bind support(internal template function).
 */
template <typename Obj, typename Rtn, typename... Args, int... Is>
std::function<Rtn(Args...)> __LLBC_BindClassMethod(Obj *obj, Rtn(Obj::*meth)(Args...), LLBC_NS LLBC_IntSequence<Is...>)
{
    return std::bind(meth, obj, LLBC_NS LLBC_PlaceHolderTemplate<Is>{}...);
}
template <typename Obj, typename Rtn, typename... Args, int... Is>
std::function<Rtn(Args...)> __LLBC_BindClassMethod(Obj *obj, Rtn(Obj::*meth)(Args...) const, LLBC_NS LLBC_IntSequence<Is...>)
{
    return std::bind(meth, obj, LLBC_NS LLBC_PlaceHolderTemplate<Is>{}...);
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

/**
 * \brief Class method bind support.
 */
template <typename Obj, typename Rtn, typename... Args>
std::function<Rtn(Args...)> LLBC_BindClassMethod(Obj *obj, Rtn(Obj::*meth)(Args...))
{
    return LLBC_INTERNAL_NS __LLBC_BindClassMethod(obj, meth, LLBC_MakeIntSequence<sizeof...(Args)>{});
}
template <typename Obj, typename Rtn, typename... Args>
std::function<Rtn(Args...)> LLBC_BindClassMethod(Obj *obj, Rtn(Obj::*meth)(Args...) const)
{
    return LLBC_INTERNAL_NS __LLBC_BindClassMethod(obj, meth, LLBC_MakeIntSequence<sizeof...(Args)>{});
}

/**
 * Floating point almost equal judge template function.
 * @param[in] x - the floating point number.
 * @param[in] y - the floating point number.
 * @return bool - return true if x almost equal y, otherwise return false.
 */
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, bool>
LLBC_IsFloatAlmostEqual(T x, T y)
{
    const T absDiff = std::abs(x - y);
    if (absDiff <= std::numeric_limits<T>::epsilon())
        return true;

    const T scale = std::max(std::abs(x), std::abs(y));
    return absDiff <= scale * std::numeric_limits<T>::epsilon();
}

/**
 * Check given floating point is zero.
 * @param[in] x - the floating point number.
 * @return bool - return true if x is zero, otherwise return false.
 */
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, bool>
LLBC_IsFloatZero(T x)
{
    return std::abs(x) <= std::numeric_limits<T>::epsilon();
}

__LLBC_NS_END