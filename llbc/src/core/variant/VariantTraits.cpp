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
#define __LLBC_INL_OBJ_TYPE_VARS_ASSIGN(ty, varName)                     \
    LLBC_Variant::ty *&lObj = left.GetMutableHolder()->data.obj.varName; \
    const LLBC_Variant::ty *rObj = right.GetHolder().data.obj.varName;   \
    if (rObj == nullptr || rObj->empty()) {                              \
        if (lObj)                                                        \
            lObj->clear();                                               \
    } else {                                                             \
        if (lObj == nullptr)                                             \
            lObj = new LLBC_Variant::ty(*rObj);                          \
        else                                                             \
            *lObj = *rObj;                                               \
    }                                                                    \

// The object type Variant equal compare macro define.
#define __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(ty, varName)                    \
    if (!right.Is##ty())                                                 \
        return false;                                                    \
    if (lHolder.data.obj.varName) {                                      \
        if (rHolder.data.obj.varName)                                    \
            return *lHolder.data.obj.varName == *rHolder.data.obj.varName; \
        else                                                             \
            return lHolder.data.obj.varName->empty();                    \
    } else {                                                             \
        if (rHolder.data.obj.varName)                                    \
            return rHolder.data.obj.varName->empty();                    \
        else                                                             \
            return true;                                                 \
    }                                                                    \

// The obj type Variant less than compare macro define.
#define __LLBC_INL_OBJ_TYPE_VARS_LT_COMP(ty, varName)                         \
    do {                                                                      \
       if (lHolder.data.obj.varName)                                          \
           return rHolder.data.obj.varName && *lHolder.data.obj.varName < *rHolder.data.obj.varName; \
                                                                              \
       return rHolder.data.obj.varName && !rHolder.data.obj.varName->empty(); \
    } while (0)                                                               \

__LLBC_NS_BEGIN

void LLBC_VariantTraits::assign(LLBC_Variant &left, const LLBC_Variant &right)
{
    if (&left == &right)
        return;

    // Become type.
    left.Become(right.GetType());

    // Execute assignment.
    if (right.IsNil())// Do NIL type data assignment.
    {
        return;
    }
    else if (right.IsRaw()) // Do RAW type data assignment.
    {
        left.GetMutableHolder()->data.raw.uint64Val = right.GetHolder().data.raw.uint64Val;
    }
    else if (right.IsStr()) // Do STR type data assignment.
    {
        __LLBC_INL_OBJ_TYPE_VARS_ASSIGN(Str, str);
    }
    else if (right.IsSeq()) // Do SEQ type data assignment.
    {
        __LLBC_INL_OBJ_TYPE_VARS_ASSIGN(Seq, seq);
    }
    else if (right.IsDict()) // Do DICT type data assignment.
    {
        __LLBC_INL_OBJ_TYPE_VARS_ASSIGN(Dict, dict);
    }
}

bool LLBC_VariantTraits::eq(const LLBC_Variant &left, const LLBC_Variant &right)
{
    if (&left == &right)
        return true;

    const LLBC_Variant::Holder &lHolder = left.GetHolder();
    const LLBC_Variant::Holder &rHolder = right.GetHolder();
    if (left.IsStr())
    {
        __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(Str, str);
    }
    else if (left.IsSeq())
    {
        __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(Seq, seq);
    }
    else if (left.IsDict())
    {
        __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP(Dict, dict);
    }
    else if (left.IsRaw())
    {
        if (!right.IsRaw())
            return false;

        if ((left.IsDouble() || left.IsFloat()) ||
            (right.IsDouble() || right.IsFloat()))
            return left.AsDouble() == right.AsDouble();

        return (left.GetHolder().data.raw.uint64Val == 
            right.GetHolder().data.raw.uint64Val);
    }

    return (left.IsNil() && right.IsNil());
}

bool LLBC_VariantTraits::ne(const LLBC_Variant &left, const LLBC_Variant &right)
{
    return !LLBC_VariantTraits::eq(left, right);
}

bool LLBC_VariantTraits::lt(const LLBC_Variant &left, const LLBC_Variant &right)
{
    if (&left == &right)
        return false;

    const LLBC_Variant::Holder &lHolder = left.GetHolder();
    const LLBC_Variant::Holder &rHolder = right.GetHolder();
    if (left.IsDict())
    {
        if (right.IsDict()) // Dict: exec compare
            __LLBC_INL_OBJ_TYPE_VARS_LT_COMP(Dict, dict);
        else // Seq/Str/Raw/Nil: false
            return false;
    }
    else if (left.IsSeq())
    {
        if (right.IsDict()) // Seq<Dict: true
            return true;
        if (right.IsSeq()) // Seq: exec compare
            __LLBC_INL_OBJ_TYPE_VARS_LT_COMP(Seq, seq);
        else // Str/Raw/Nil: false
            return false;
    }
    else if (left.IsStr())
    {
        if (right.IsDict() ||
            right.IsSeq()) // Dict/Seq: true
            return true;
        else if (right.IsStr()) // Str: exec compare
            __LLBC_INL_OBJ_TYPE_VARS_LT_COMP(Str, str);
        else // Raw/Nil: false
            return false;
    }
    else if (left.IsRaw())
    {
        if (right.IsDict() ||
            right.IsSeq() ||
            right.IsStr()) // Dict/Seq/Str: true
        {
            return true;
        }
        else if (right.IsRaw()) // Raw: exec compare
        {
            if ((left.IsDouble() || left.IsFloat()) ||
                (right.IsDouble() || right.IsFloat()))
                return left.AsDouble() < right.AsDouble();

            if (left.IsUnsignedRaw() || right.IsUnsignedRaw())
                return left.AsUInt64() < right.AsUInt64();
            else
                return left.AsInt64() < right.AsInt64();
        }
        else // Nil: false
        {
            return false;
        }
    }
    else
    {
        return !right.IsNil();
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
    LLBC_Variant ret;
    assign(ret, left);
    add_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::sub(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret;
    assign(ret, left);
    sub_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::mul(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret;
    assign(ret, left);
    mul_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::div(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret;
    assign(ret, left);
    div_equal(ret, right);
    return ret;
}

void LLBC_VariantTraits::add_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    // &Left == &Right rules:
    if (&left == &right)
    {
        LLBC_Variant clone;
        assign(clone, left);
        add_equal(left, clone);

        return;
    }

    // Left is Nil rules:
    // > Left[Nil] + Right[Any] = Right
    if (left.IsNil())
    {
        assign(left, right);
        return;
    }

    // Left is Dict rules:
    // > Left[Dict] + Right[Dict] = Dict[Left join Right]
    // > Left[Dict] + Right[Non Dict] = Left
    if (left.IsDict())
    {
        if (right.IsDict())
        {
            LLBC_Variant::Dict *&lDict = left.GetMutableHolder()->data.obj.dict;
            const LLBC_Variant::Dict *rDict = right.GetHolder().data.obj.dict;
            if (!rDict || rDict->empty())
                return;

            if (lDict)
                lDict->insert(rDict->begin(), rDict->end());
            else
                lDict = new LLBC_Variant::Dict(*rDict);
        }

        return;
    }

    // Left is Seq rules:
    // > Left[Seq] + Right[Dict] = Seq[Left join Right.Keys]
    // > Left[Seq] + Right[Seq] = Seq[Left join Right]
    // > Left[Seq] + Right[Non Seq/Dict] = Left Append Right
    if (left.IsSeq())
    {
        LLBC_Variant::Seq *&lSeq = left.GetMutableHolder()->data.obj.seq;
        if (right.IsDict())
        {
            const LLBC_Variant::Dict *rDict = right.GetHolder().data.obj.dict;
            if (!rDict || rDict->empty())
                return;

            if (!lSeq)
                lSeq = new LLBC_Variant::Seq;
            if (lSeq->capacity() < lSeq->size() + rDict->size())
                lSeq->reserve(lSeq->size() + rDict->size());

            LLBC_Variant::DictConstIter rEndIt = rDict->end();
            for (LLBC_Variant::DictConstIter rIt = rDict->begin(); rIt != rEndIt; ++rIt)
                lSeq->push_back(rIt->first);
        }
        else if (right.IsSeq())
        {
            const LLBC_Variant::Seq *rSeq = right.GetHolder().data.obj.seq;
            if (!rSeq || rSeq->empty())
                return;

            if (lSeq)
                lSeq->insert(lSeq->end(), rSeq->begin(), rSeq->end());
            else
                lSeq = new LLBC_Variant::Seq(*rSeq);
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
    // Left[Str] + Right[Raw/Nil] = Left + Right.AsStr()
    if (left.IsStr())
    {
        LLBC_Variant::Str *&lStr = left.GetMutableHolder()->data.obj.str;
        if (right.IsDict() || right.IsSeq())
        {
            return;
        }
        else if (right.IsStr())
        {
            LLBC_Variant::Str *rStr = right.GetHolder().data.obj.str;
            if (!rStr || rStr->empty())
                return;

            if (lStr)
                lStr->append(*rStr);
            else
                lStr = new LLBC_Variant::Str(*rStr);
        }
        else
        {
            if (lStr)
                lStr->append(right.AsStr());
            else
                lStr = new LLBC_Variant::Str(right.AsStr());
        }

        return;
    }

    // Left is Raw rules:
    // > Left[Raw] + Right[Dict/Seq/Nil] = Left
    // > Left[Raw] + Right[Str] = Left.AsStr() + Right
    // > Left[Raw] + Right[Raw] = VariantArithmetic::Performs(l, r, op)
    if (left.IsRaw())
    {
        if (right.IsStr())
        {
            left = left.AsStr();
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
    if (&left == &right)
    {
        LLBC_Variant clone;
        assign(clone, left);
        sub_equal(left, clone);

        return;
    }

    // Left is Nil rules:
    // > Left[Nil] - Right[Any] = Left
    if (left.IsNil())
        return;

    // Left is Dict rules:
    // > Left[Dict] - Right[Dict/Seq] = Dict[Left - Right]
    // > Left[Dict] - Right[Non Dict/Seq] = Dict[Left -Right(as key)]
    if (left.IsDict())
    {
        LLBC_Variant::Dict *&lDict = left.GetMutableHolder()->data.obj.dict;
        if (!lDict || lDict->empty())
            return;

        if (right.IsDict())
        {
            const LLBC_Variant::Dict *rDict = right.GetHolder().data.obj.dict;
            if (!rDict || rDict->empty())
                return;

            const LLBC_Variant::DictConstIter rEndIt = rDict->end();
            for (LLBC_Variant::DictConstIter rIt = rDict->begin(); rIt != rEndIt; ++rIt)
                lDict->erase(rIt->first);
        }
        else if (right.IsSeq())
        {
            const LLBC_Variant::Seq *rSeq = right.GetHolder().data.obj.seq;
            if (!rSeq || rSeq->empty())
                return;

            const LLBC_Variant::SeqConstIter rEndIt = rSeq->end();
            for (LLBC_Variant::SeqConstIter rIt = rSeq->begin(); rIt != rEndIt; ++rIt)
            {
                lDict->erase(*rIt);
                if (lDict->empty())
                    return;
            }
        }
        else
        {
            lDict->erase(right);
        }

        return;
    }

    // Left is Seq rules:
    // > Left[Seq] - Right[Dict] = Seq[Left - Right.Keys]
    // > Left[Seq] - Right[Seq] = Seq[Left - Right]
    // > Left[Seq] - Right[Non Seq/Dict] = Left - Right(as left element)
    if (left.IsSeq())
    {
        LLBC_Variant::Seq *&lSeq = left.GetMutableHolder()->data.obj.seq;
        if (!lSeq || lSeq->empty())
            return;

        if (right.IsDict())
        {
            const LLBC_Variant::Dict *rDict = right.GetHolder().data.obj.dict;
            if (!rDict || rDict->empty())
                return;

            LLBC_Variant::DictConstIter rEndIt = rDict->end();
            for (LLBC_Variant::DictConstIter rIt = rDict->begin(); rEndIt != rDict->end(); ++rIt)
            {
                lSeq->erase(std::remove_if(lSeq->begin(), lSeq->end(), 
                            [&rIt](const LLBC_Variant &elem) { return elem == rIt->second; }), lSeq->end());
                if (lSeq->empty())
                    return;
            }
        }
        else if (right.IsSeq())
        {

            const LLBC_Variant::Seq *rSeq = right.GetHolder().data.obj.seq;
            if (!rSeq || rSeq->empty())
                return;

            LLBC_Variant::SeqConstIter rItEnd = rSeq->end();
            for (LLBC_Variant::SeqConstIter rIt = rSeq->begin(); rIt != rItEnd; ++rIt)
            {
                lSeq->erase(std::remove_if(lSeq->begin(), lSeq->end(), 
                            [&rIt](const LLBC_Variant &elem) { return elem == *rIt; }), lSeq->end());
                if (lSeq->empty())
                    return;
            }
        }
        else
        {
            lSeq->erase(std::remove_if(lSeq->begin(), lSeq->end(),
                        [&right](const LLBC_Variant &elem) { return elem == right; }), lSeq->end());
        }

        return;
    }

    // Left is Str rules:
    // > Left[Str] - Right[Nil/Seq/Dict] = Left
    // Left[Str] - Right[Str] = Left str - Right str
    // Left[Str] - Right[Raw] = Left - Right.AsStr()
    if (left.IsStr())
    {
        LLBC_Variant::Str *&lStr = left.GetMutableHolder()->data.obj.str;
        if (!lStr || lStr->empty())
            return;

        if (right.IsStr())
        {
            const LLBC_Variant::Str *rStr = right.GetHolder().data.obj.str;
            if (!rStr || rStr->empty())
                return;

            LLBC_String::size_type pos = lStr->find(*rStr);
            while (pos != LLBC_String::npos)
            {
                lStr->erase(pos, rStr->size());
                pos = lStr->find(*rStr);
            }
        }
        else if (right.IsRaw())
        {
            const LLBC_String rStr = right.AsStr();
            LLBC_String::size_type pos = lStr->find(rStr);
            while (pos != LLBC_String::npos)
            {
                lStr->erase(pos, rStr.size());
                pos = lStr->find(rStr);
            }
        }

        return;
    }

    // Left is Raw rules:
    // Left[Raw] - Right[Dict/Seq/Nil] = Left
    // Left[Raw] - Right[Str] = Left.AsStr() - Right
    // Left[Raw] - Right[Raw] = VariantArithmetic::Performs(l, r, op)
    if (left.IsRaw())
    {
        if (right.IsStr())
        {
            left = left.AsStr();
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
    if (&left == &right)
    {
        LLBC_Variant clone;
        assign(clone, left);
        mul_equal(left, clone);

        return;
    }

    // Left or Right is Nil rules:
    // > Left[Nil] or Right[Nil] mul = Nil
    if (left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }

    // Left is Dict rules:
    // > Left[Dict] * Right[Dict/Seq] = Intersection Of Left and Right
    // > Left[Dict] * Right[Str/Raw] = Left[Dict]
    else if (left.IsDict())
    {
        LLBC_Variant::Dict *lDict = left.GetMutableHolder()->data.obj.dict;
        if (!lDict || lDict->empty())
            return;

        if (right.IsSeq() || right.IsDict())
        {
            if (right.IsEmpty())
            {
                lDict->clear();
                return;
            }

            for (LLBC_Variant::DictIter lIt = lDict->begin(); lIt != lDict->end(); )
            {
                if ((right.IsDict() && right.DictFind(lIt->first) == right.DictEnd()) ||
                    (right.IsSeq() && std::find(right.SeqBegin(), right.SeqEnd(), lIt->first) == right.SeqEnd()))
                    lDict->erase(lIt++);
                else
                    ++lIt;
            }
        }

        return;
    }

    // Left is Seq rules:
    // > Left[Seq] * Right[Dict/Seq] = Intersection On Left and Right
    // > Left[Seq] * Right[Str] = Left
    // > Left[Seq] * Right[Raw] = Left[Seq] repeat right.AsInt32() times
    else if (left.IsSeq())
    {
        LLBC_Variant::Seq *lSeq = left.GetMutableHolder()->data.obj.seq;
        if (!lSeq || lSeq->empty())
            return;

        if (right.IsSeq() || right.IsDict())
        {
            if (right.IsEmpty())
            {
                lSeq->clear();
                return;
            }

            long lSeqSize = static_cast<long>(lSeq->size());
            for (long lIdx = lSeqSize - 1; lIdx >= 0; --lIdx)
            {
                if ((right.IsDict() && right.DictFind(lSeq->at(lIdx)) == right.DictEnd()) ||
                    (right.IsSeq() && std::find(right.SeqBegin(), right.SeqEnd(), lSeq->at(lIdx)) == right.SeqEnd()))
                    lSeq->erase(lSeq->begin() + lIdx);
            }
        }
        else if (right.IsRaw())
        {
            sint32 rRaw = right.AsInt32();
            if (rRaw <= 0)
            {
                lSeq->clear();
                return;
            }
            else if (rRaw == 1)
            {
                return;
            }

            size_t lSeqReserve = lSeq->size() * rRaw;
            if (lSeq->capacity() < lSeqReserve)
                lSeq->reserve(lSeqReserve);

            for (sint32 lIdx = 1; lIdx < rRaw; ++lIdx)
                lSeq->insert(lSeq->end(), lSeq->begin(), lSeq->end());
        }

        return;
    }

    // Left is Str rules:
    // > Left[Str] * Right[Dict/Seq] = Right[Dict/Seq] * Left[Str]
    // > Left[Str] * Right[Str] = Left
    // > Left[Str] * Right[Str] = Left[Str] repeat Right.AsInt32() times
    else if (left.IsStr())
    {
        if (right.IsDict() || right.IsSeq())
        {
            left = mul(right, left);
        }
        else if (right.IsRaw())
        {
            LLBC_Variant::Str *lStr = left.GetMutableHolder()->data.obj.str;
            if (!lStr || lStr->empty())
                return;

            *lStr *= right.AsInt32();
        }

        return;
    }

    // Left is Raw rules:
    // > Left[Raw] * Right[Dict/Seq/Str] = Right[Dict/Seq/Str] * Left[Raw]
    // > Left[Raw] * Right[Raw] = Left[left raw * right raw]
    else if (left.IsRaw())
    {
        if (right.IsDict() || right.IsSeq() || right.IsStr())
        {
            left = mul(right, left);
        }
        else if (right.IsRaw())
        {
            LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_MUL);
        }

        return;
    }
}

void LLBC_VariantTraits::div_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    // &Left == &Right rules:
    if (&left == &right)
    {
        LLBC_Variant clone;
        assign(clone, left);
        div_equal(left, clone);

        return;
    }

    // Left[Nil] or Right[Nil] = Nil
    if (left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }

    // Left[Dict/Seq/Str] / Right[Non Nil] = Left # undefined
    if (!left.IsRaw())
        return;

    // Left[Raw] / Right[Dict/Seq/Str] = Left[Raw]
    if (!right.IsRaw())
        return;

    // Left[Raw] / Right[Raw] = VariantArithmetic(l, r, op)
    LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_DIV);
}

__LLBC_NS_END

#undef __LLBC_INL_OBJ_TYPE_VARS_ASSIGN
#undef __LLBC_INL_OBJ_TYPE_VARS_EQ_COMP
#undef __LLBC_INL_OBJ_TYPE_VARS_LT_COMP
