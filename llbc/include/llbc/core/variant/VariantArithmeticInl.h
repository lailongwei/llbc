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


__LLBC_NS_BEGIN

template <>
inline decltype(bool() + bool()) LLBC_VariantArithmetic::Performs_raw_operation(bool left, bool right, int type)
{
    typedef decltype(bool() + bool()) _RtnType;
    switch (type)
    {
    case VT_ARITHMETIC_ADD:
        return left + right;

    case VT_ARITHMETIC_SUB:
        return left - right;

    case VT_ARITHMETIC_MUL:
        return left * right;
    case VT_ARITHMETIC_DIV:
        // Disable C4804 compiler warning if using MSVC compiler.
        // Compiler Warning (level 1) C4804:
        // - https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4804?view=msvc-170
        #if LLBC_CUR_COMP == LLBC_COMP_MSVC
        #pragma warning(push)
        #pragma warning(disable: 4804)
        #endif // MSVC compiler.

        return right ? left / right : _RtnType();

        #if LLBC_CUR_COMP == LLBC_COMP_MSVC
        #pragma warning(pop)
        #endif // MSVC compiler.

    case VT_ARITHMETIC_MOD:
        // Disable C4804 compiler warning if using MSVC compiler.
        // Compiler Warning (level 1) C4804:
        // - https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4804?view=msvc-170
        #if LLBC_CUR_COMP == LLBC_COMP_MSVC
        #pragma warning(push)
        #pragma warning(disable: 4804)
        #endif // MSVC compiler.

        return right ? left % right : _RtnType();

        #if LLBC_CUR_COMP == LLBC_COMP_MSVC
        #pragma warning(pop)
        #endif // MSVC compiler.

    default:
        break;
    }

    return _RtnType();
}

template <>
inline void *LLBC_VariantArithmetic::Performs_raw_operation(void *left, void *right, int type)
{
    sint64 leftVal = 0, rightVal = 0;
    memcpy(&leftVal, &left, sizeof(void *));
    memcpy(&rightVal, &right, sizeof(void *));

    switch (type)
    {
    case VT_ARITHMETIC_ADD:
        leftVal += rightVal;
        break;

    case VT_ARITHMETIC_SUB:
        leftVal -= rightVal;
        break;

    case VT_ARITHMETIC_MUL:
        leftVal *= rightVal;
        break;

    case VT_ARITHMETIC_DIV:
        leftVal /= rightVal;
        break;

    case VT_ARITHMETIC_MOD:
        leftVal %= rightVal;
        break;

    default:
        break;
    }

    void *finalPtr = nullptr;
    memcpy(&finalPtr, &leftVal, sizeof(void *));

    return finalPtr;
}

template <>
inline float LLBC_VariantArithmetic::Performs_raw_operation(float left, float right, int type)
{
    switch (type)
    {
    case VT_ARITHMETIC_ADD:
        return left + right;

    case VT_ARITHMETIC_SUB:
        return left - right;

    case VT_ARITHMETIC_MUL:
        return left * right;

    case VT_ARITHMETIC_DIV:
        if (LIKELY(!LLBC_IsFloatZero(right)))
            return left / right;
        else
            return 0.0f;

    case VT_ARITHMETIC_MOD:
    {
        const auto castedLeft = static_cast<sint64>(left);
        const auto castedRight = static_cast<sint64>(right);
        if (LIKELY(castedRight != 0))
            return static_cast<float>(castedLeft % castedRight);
        else
            return static_cast<float>(castedLeft);
    }

    default:
        break;
    }

    return float();
}

template <>
inline double LLBC_VariantArithmetic::Performs_raw_operation(double left, double right, int type)
{
    switch (type)
    {
    case VT_ARITHMETIC_ADD:
        return left + right;

    case VT_ARITHMETIC_SUB:
        return left - right;

    case VT_ARITHMETIC_MUL:
        return left * right;

    case VT_ARITHMETIC_DIV:
        if (LIKELY(!LLBC_IsFloatZero(right)))
            return left / right;
        else
            return 0.0;

    case VT_ARITHMETIC_MOD:
    {
        const auto castedLeft = static_cast<sint64>(left);
        const auto castedRight = static_cast<sint64>(right);
        if (LIKELY(castedRight != 0))
            return static_cast<double>(castedLeft % castedRight);
        else
            return static_cast<double>(castedLeft);
    }

    default:
        break;
    }

    return double();
}

template <>
inline ldouble LLBC_VariantArithmetic::Performs_raw_operation(ldouble left, ldouble right, int type)
{
    switch (type)
    {
    case VT_ARITHMETIC_ADD:
        return left + right;

    case VT_ARITHMETIC_SUB:
        return left - right;

    case VT_ARITHMETIC_MUL:
        return left * right;

    case VT_ARITHMETIC_DIV:
        if (LIKELY(!LLBC_IsFloatZero(right)))
            return left / right;
        else
            return 0.0;

    case VT_ARITHMETIC_MOD:
    {
        const auto castedLeft = static_cast<sint64>(left);
        const auto castedRight = static_cast<sint64>(right);
        if (LIKELY(castedRight != 0))
            return static_cast<ldouble>(castedLeft % castedRight);
        else
            return static_cast<ldouble>(castedLeft);
    }

    default:
        break;
    }

    return ldouble();
}

template <typename RawType, typename RtnType>
RtnType LLBC_VariantArithmetic::Performs_raw_operation(RawType left, RawType right, int type)
{
    switch (type)
    {
    case VT_ARITHMETIC_ADD:
        return left + right;

    case VT_ARITHMETIC_SUB:
        return left - right;

    case VT_ARITHMETIC_MUL:
        return left * right;

    case VT_ARITHMETIC_DIV:
        if (LIKELY(right != RawType()))
            return left / right;
        else
            return RtnType();

    case VT_ARITHMETIC_MOD:
        if (LIKELY(right != RawType()))
            return left % right;
        else
            return RtnType();

    default:
        return RtnType();
    }
}

__LLBC_NS_END
