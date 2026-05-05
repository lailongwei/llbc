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


#include "llbc/common/Export.h"

#include "llbc/core/variant/Variant.h"
#include "llbc/core/variant/VariantArithmetic.h"
#include "llbc/core/variant/VariantTraits.h"

// Some file level internal macros define
// The object type Variant assignment macro define.

// The object type Variant equal compare macro define.
#define __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(ty, varName) \
    if (!right.Is<ty>())                              \
        return false;                                 \
    return lData.varName() == rData.varName()         \

__LLBC_NS_BEGIN

bool LLBC_VariantTraits::eq(const LLBC_Variant &left, const LLBC_Variant &right)
{
    if (&left == &right)
        return true;

    const auto &lData = left._data;
    const auto &rData = right._data;
    if (left.Is<LLBC_Variant::Str>()) // Str == Any
    {
        // Str == Non-Raw
        if (!right.IsRaw())
        {
            __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(LLBC_Variant::Str, str);
        }
        else // Str == Raw
        {
            if (right.Is<float, double>())
                return LLBC_IsFloatAlmostEqual(left.As<double>(), right.As<double>());
            else if (right.IsUnsigned())
                return left.As<uint64>() == right.As<uint64>();
            else
                return left.As<sint64>() == right.As<sint64>();
        }
    }
    else if (left.Is<LLBC_Variant::Seq>()) // Seq == Any
    {
        __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(LLBC_Variant::Seq, seq);
    }
    else if (left.Is<LLBC_Variant::Dict>()) // Dict == Any
    {
        __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(LLBC_Variant::Dict, dict);
    }
    else if (left.IsRaw()) // Raw == Any
    {
        if (!right.IsRaw() && !right.Is<LLBC_Variant::Str>()) // Raw == Non-Raw&&Non-Str
            return false;

        // Raw == Raw/Str
        if (left.Is<float, double>() || right.Is<float, double>())
            return LLBC_IsFloatAlmostEqual(left.As<double>(), right.As<double>());
        else if (left.IsUnsigned() || right.IsUnsigned())
            return left.As<uint64>() == right.As<uint64>();
        else
            return left.As<sint64>() == right.As<sint64>();
    }

    return (left.Is<void>() && right.Is<void>());
}

bool LLBC_VariantTraits::ne(const LLBC_Variant &left, const LLBC_Variant &right)
{
    return !LLBC_VariantTraits::eq(left, right);
}

bool LLBC_VariantTraits::lt(const LLBC_Variant &left, const LLBC_Variant &right)
{
    if (&left == &right)
        return false;

    const auto &lData = left._data;
    const auto &rData = right._data;
    if (left.Is<LLBC_Variant::Dict>())
    {
        if (right.Is<LLBC_Variant::Dict>()) // Dict: exec compare
            return lData.dict() < rData.dict();
        else // Seq/Str/Raw/Nil: false
            return false;
    }
    else if (left.Is<LLBC_Variant::Seq>())
    {
        if (right.Is<LLBC_Variant::Dict>()) // Seq<Dict: true
            return true;
        if (right.Is<LLBC_Variant::Seq>()) // Seq: exec compare
            return lData.seq() < rData.seq();
        else // Str/Raw/Nil: false
            return false;
    }
    else if (left.Is<LLBC_Variant::Str>())
    {
        if (right.Is<LLBC_Variant::Dict>() || right.Is<LLBC_Variant::Seq>()) // Dict/Seq: true
        {
            return true;
        }
        else if (right.Is<LLBC_Variant::Str>()) // Str: exec compare
        {
            return lData.str() < rData.str();
        }
        else if (right.IsRaw()) // Raw: exec compare(convert to raw)
        {
            if (right.Is<float, double>())
                return left.As<double>() < right.As<double>();
            else if (right.IsUnsigned())
                return left.As<uint64>() < right.As<uint64>();
            else
                return left.As<sint64>() < right.As<sint64>();
        }
        else // Nil: false
        {
            return false;
        }
    }
    else if (left.IsRaw())
    {
        if (right.Is<LLBC_Variant::Dict>() || right.Is<LLBC_Variant::Seq>()) // Dict/Seq: true
        {
            return true;
        }
        else if (right.Is<LLBC_Variant::Str>() || right.IsRaw()) // Str/Raw: exec compare
        {
            if (left.Is<float, double>() || right.Is<float, double>())
                return left.As<double>() < right.As<double>();
            else if (left.IsUnsigned() || right.IsUnsigned())
                return left.As<uint64>() < right.As<uint64>();
            else
                return left.As<sint64>() < right.As<sint64>();
        }
        else // Nil: false
        {
            return false;
        }
    }
    else
    {
        return !right.Is<void>();
    }
}

bool LLBC_VariantTraits::gt(const LLBC_Variant &left, const LLBC_Variant &right)
{
    return right < left;
}

bool LLBC_VariantTraits::le(const LLBC_Variant &left, const LLBC_Variant &right)
{
    return !(right < left);
}

bool LLBC_VariantTraits::ge(const LLBC_Variant &left, const LLBC_Variant &right)
{
    return !(left < right);
}

LLBC_Variant LLBC_VariantTraits::add(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret(left);
    add_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::sub(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret(left);
    sub_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::mul(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret(left);
    mul_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::div(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret(left);
    div_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::mod(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret(left);
    mod_equal(ret, right);
    return ret;
}

void LLBC_VariantTraits::add_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    // &Left == &Right rules:
    if (UNLIKELY(&left == &right))
    {
        const LLBC_Variant clone(left);
        add_equal(left, clone);

        return;
    }

    // Left is Nil rules:
    // > Left[Nil] + Right[Any] = Right
    if (left.Is<void>())
    {
        left = right;
        return;
    }

    // Left is Dict rules:
    // > Left[Dict] + Right[Dict] = Dict[Left join Right]
    // > Left[Dict] + Right[Non Dict] = Left
    if (left.Is<LLBC_Variant::Dict>())
    {
        if (right.Is<LLBC_Variant::Dict>())
        {
            auto &lDict = left._data.dict();
            const auto &rDict = right._data.dict();
            if (rDict.empty())
                return;

            lDict.insert(rDict.begin(), rDict.end());
        }

        return;
    }

    // Left is Seq rules:
    // > Left[Seq] + Right[Dict] = Seq[Left join Right.Keys]
    // > Left[Seq] + Right[Seq] = Seq[Left join Right]
    // > Left[Seq] + Right[Non Seq/Dict] = Left Append Right
    if (left.Is<LLBC_Variant::Seq>())
    {
        auto &lSeq = left._data.seq();
        if (right.Is<LLBC_Variant::Dict>())
        {
            const auto &rDict = right._data.dict();
            if (rDict.empty())
                return;

            if (lSeq.capacity() < lSeq.size() + rDict.size())
                lSeq.reserve(lSeq.size() + rDict.size());

            const auto rEndIt = rDict.end();
            for (auto rIt = rDict.begin(); rIt != rEndIt; ++rIt)
                lSeq.push_back(rIt->first);
        }
        else if (right.Is<LLBC_Variant::Seq>())
        {
            const auto &rSeq = right._data.seq();
            if (rSeq.empty())
                return;

            lSeq.insert(lSeq.end(), rSeq.begin(), rSeq.end());
        }
        else
        {
            left.SeqPushBack(right);
        }

        return;
    }

    // Left is Str rules:
    // Left[Str] + Right[Dict/Seq] = Left
    // Left[Str] + Right[Str] = Left[left str + right str]
    // Left[Str] + Right[Raw/Nil] = Left + Right.As<Str>()
    if (left.Is<LLBC_Variant::Str>())
    {
        auto &lStr = left._data.str();
        if (right.Is<LLBC_Variant::Dict>() || right.Is<LLBC_Variant::Seq>())
        {
            return;
        }
        else if (right.Is<LLBC_Variant::Str>())
        {
            const auto &rStr = right._data.str();
            if (rStr.empty())
                return;

            lStr.append(rStr);
        }
        else
        {
            lStr.append(right.As<LLBC_Variant::Str>());
        }

        return;
    }

    // Left is Raw rules:
    // > Left[Raw] + Right[Dict/Seq/Nil] = Left
    // > Left[Raw] + Right[Str] = Left.As<Str>() + Right
    // > Left[Raw] + Right[Raw] = VariantArithmetic::Performs(l, r, op)
    if (left.IsRaw())
    {
        if (right.Is<LLBC_Variant::Str>())
        {
            left = left.As<LLBC_Variant::Str>();
            add_equal(left, right);
            return;
        }
        else if (right.IsRaw())
        {
            LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_ADD);
        }

        return;
    }
}

void LLBC_VariantTraits::sub_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    // &Left == &Right rules:
    if (UNLIKELY(&left == &right))
    {
        const LLBC_Variant clone(left);
        sub_equal(left, clone);

        return;
    }

    // Left is Nil rules:
    // > Left[Nil] - Right[Any] = Left
    if (left.Is<void>())
        return;

    // Left is Dict rules:
    // > Left[Dict] - Right[Dict/Seq] = Dict[Left - Right]
    // > Left[Dict] - Right[Non Dict/Seq] = Dict[Left -Right(as key)]
    if (left.Is<LLBC_Variant::Dict>())
    {
        auto &lDict = left._data.dict();
        if (lDict.empty())
            return;

        if (right.Is<LLBC_Variant::Dict>())
        {
            const auto &rDict = right._data.dict();
            if (rDict.empty())
                return;

            const auto rEndIt = rDict.end();
            for (auto rIt = rDict.begin(); rIt != rEndIt; ++rIt)
                lDict.erase(rIt->first);
        }
        else if (right.Is<LLBC_Variant::Seq>())
        {
            const auto &rSeq = right._data.seq();
            if (rSeq.empty())
                return;

            const auto rEndIt = rSeq.end();
            for (auto rIt = rSeq.begin(); rIt != rEndIt; ++rIt)
            {
                lDict.erase(*rIt);
                if (lDict.empty())
                    return;
            }
        }
        else
        {
            lDict.erase(right);
        }

        return;
    }

    // Left is Seq rules:
    // > Left[Seq] - Right[Dict] = Seq[Left - Right.Keys]
    // > Left[Seq] - Right[Seq] = Seq[Left - Right]
    // > Left[Seq] - Right[Non Seq/Dict] = Left - Right(as left element)
    if (left.Is<LLBC_Variant::Seq>())
    {
        auto &lSeq = left._data.seq();
        if (lSeq.empty())
            return;

        if (right.Is<LLBC_Variant::Dict>())
        {
            const auto &rDict = right._data.dict();
            if (rDict.empty())
                return;

            const auto rEndIt = rDict.end();
            for (auto rIt = rDict.begin(); rIt != rEndIt; ++rIt)
            {
                lSeq.erase(std::remove_if(lSeq.begin(), lSeq.end(),
                            [&rIt](const LLBC_Variant &elem) { return elem == rIt->first; }), lSeq.end());
                if (lSeq.empty())
                    return;
            }
        }
        else if (right.Is<LLBC_Variant::Seq>())
        {

            const auto &rSeq = right._data.seq();
            if (rSeq.empty())
                return;

            auto rItEnd = rSeq.end();
            for (auto rIt = rSeq.begin(); rIt != rItEnd; ++rIt)
            {
                lSeq.erase(std::remove_if(lSeq.begin(), lSeq.end(),
                            [&rIt](const LLBC_Variant &elem) { return elem == *rIt; }), lSeq.end());
                if (lSeq.empty())
                    return;
            }
        }
        else
        {
            lSeq.erase(std::remove_if(lSeq.begin(), lSeq.end(),
                        [&right](const LLBC_Variant &elem) { return elem == right; }), lSeq.end());
        }

        return;
    }

    // Left is Str rules:
    // > Left[Str] - Right[Nil/Seq/Dict] = Left
    // > Left[Str] - Right[Str] = Left str - Right str
    // > Left[Str] - Right[Raw] = Left - Right.As<Str>()
    if (left.Is<LLBC_Variant::Str>())
    {
        auto &lStr = left._data.str();
        if (lStr.empty())
            return;

        if (right.Is<LLBC_Variant::Str>())
        {
            const auto &rStr = right._data.str();
            if (rStr.empty())
                return;

            LLBC_Variant::Str::size_type pos = lStr.find(rStr);
            while (pos != LLBC_Variant::Str::npos)
            {
                lStr.erase(pos, rStr.size());
                pos = lStr.find(rStr);
            }
        }
        else if (right.IsRaw())
        {
            sub_equal(left, LLBC_Variant(right.As<LLBC_Variant::Str>()));
        }

        return;
    }

    // Left is Raw rules:
    // > Left[Raw] - Right[Dict/Seq/Nil] = Left
    // > Left[Raw] - Right[Str] = Left.As<Str>() - Right
    // > Left[Raw] - Right[Raw] = VariantArithmetic::Performs(l, r, op)
    if (left.IsRaw())
    {
        if (right.Is<LLBC_Variant::Str>())
        {
            left = left.As<LLBC_Variant::Str>();
            sub_equal(left, right);
        }
        else if (right.IsRaw())
        {
            LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_SUB);
        }

        return;
    }
}

void LLBC_VariantTraits::mul_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    // &Left == &Right rules:
    if (UNLIKELY(&left == &right))
    {
        const LLBC_Variant clone(left);
        mul_equal(left, clone);

        return;
    }

    // Left or Right is Nil rules:
    // > Left[Nil] or Right[Nil] mul = Nil
    if (left.Is<void>() || right.Is<void>())
    {
        left.Become<void>();
        return;
    }

    // Left is Dict rules:
    // > Left[Dict] * Right[Dict/Seq] = Intersection of Left and Right
    // > Left[Dict] * Right[Str/Raw] = Left[Dict]
    if (left.Is<LLBC_Variant::Dict>())
    {
        LLBC_Variant::Dict &lDict = left._data.dict();
        if (lDict.empty())
            return;

        if (right.Is<LLBC_Variant::Seq>() || right.Is<LLBC_Variant::Dict>())
        {
            if (right.IsEmpty())
            {
                lDict.clear();
                return;
            }

            for (auto lIt = lDict.begin(); lIt != lDict.end(); )
            {
                if ((right.Is<LLBC_Variant::Dict>() && right.DictFind(lIt->first) == right.DictEnd()) ||
                    (right.Is<LLBC_Variant::Seq>() && std::find(right.SeqBegin(), right.SeqEnd(), lIt->first) == right.SeqEnd()))
                    lDict.erase(lIt++);
                else
                    ++lIt;
            }
        }

        return;
    }

    // Left is Seq rules:
    // > Left[Seq] * Right[Dict/Seq] = Intersection On Left and Right
    // > Left[Seq] * Right[Str] = Left
    // > Left[Seq] * Right[Raw] = Left[Seq] repeat right.As<sint32>() times
    else if (left.Is<LLBC_Variant::Seq>())
    {
        auto &lSeq = left._data.seq();
        if (lSeq.empty())
            return;

        if (right.Is<LLBC_Variant::Seq>() || right.Is<LLBC_Variant::Dict>())
        {
            if (right.IsEmpty())
            {
                lSeq.clear();
                return;
            }

            const auto lSeqSize = static_cast<sint64>(lSeq.size());
            for (sint64 lIdx = lSeqSize - 1; lIdx >= 0; --lIdx)
            {
                if ((right.Is<LLBC_Variant::Dict>() && right.DictFind(lSeq.at(lIdx)) == right.DictEnd()) ||
                    (right.Is<LLBC_Variant::Seq>() && std::find(right.SeqBegin(), right.SeqEnd(), lSeq.at(lIdx)) == right.SeqEnd()))
                    lSeq.erase(lSeq.begin() + lIdx);
            }
        }
        else if (right.IsRaw())
        {
            sint32 rRaw = right.As<sint32>();
            if (rRaw <= 0)
            {
                lSeq.clear();
                return;
            }
            else if (rRaw == 1)
            {
                return;
            }

            size_t lSeqReserve = lSeq.size() * rRaw;
            if (lSeq.capacity() < lSeqReserve)
                lSeq.reserve(lSeqReserve);

            const auto lSeqEnd = lSeq.end();
            for (sint32 rptIdx = 1; rptIdx < rRaw; ++rptIdx)
                lSeq.insert(lSeq.end(), lSeq.begin(), lSeqEnd);
        }

        return;
    }

    // Left is Str rules:
    // > Left[Str] * Right[Dict/Seq] = Left[Str]
    // > Left[Str] * Right[Str] = Left[Str]
    // > Left[Str] * Right[Raw] = Left[Str] repeat Right.As<sint32>() times
    else if (left.Is<LLBC_Variant::Str>())
    {
        if (right.IsRaw())
        {
            auto &lStr = left._data.str();
            if (lStr.empty())
                return;

            lStr *= right.As<sint32>();
        }

        return;
    }

    // Left is Raw rules:
    // > Left[Raw] * Right[Dict/Seq/Str] = Left[Raw]
    // > Left[Raw] * Right[Raw] = Left[left raw * right raw]
    else if (left.IsRaw())
    {
        if (right.IsRaw())
        {
            LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_MUL);
        }

        return;
    }
}

void LLBC_VariantTraits::div_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    // &Left == &Right rules:
    if (UNLIKELY(&left == &right))
    {
        const LLBC_Variant clone(left);
        div_equal(left, clone);

        return;
    }

    // Left[Nil] or Right[Nil] = Nil
    if (left.Is<void>() || right.Is<void>())
    {
        left.Become<void>();
        return;
    }

    // Left[Dict/Seq/Str] / Right[Dict/Seq/Str] = Left # undefined
    if (!left.IsRaw() || !right.IsRaw())
        return;

    // Left[Raw] / Right[Raw] = VariantArithmetic(l, r, op)
    LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_DIV);
}

void LLBC_VariantTraits::mod_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    // &Left == &Right rules:
    if (UNLIKELY(&left == &right))
    {
        const LLBC_Variant clone(left);
        mod_equal(left, clone);

        return;
    }

    // Left[Nil] or Right[Nil] = Nil
    if (left.Is<void>() || right.Is<void>())
    {
        left.Become<void>();
        return;
    }

    // Left[Dict/Seq/Str] / Right[Dict/Seq/Str] = Left # undefined
    if (!left.IsRaw() || !right.IsRaw())
        return;

    // Left[Raw] / Right[Raw] = VariantArithmetic(l, r, op)
    LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_MOD);
}

__LLBC_NS_END

#undef __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP
