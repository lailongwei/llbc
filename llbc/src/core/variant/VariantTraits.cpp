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
#include "llbc/common/BeforeIncl.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/variant/Variant.h"
#include "llbc/core/variant/VariantArithmetic.h"
#include "llbc/core/variant/VariantTraits.h"

__LLBC_NS_BEGIN

void LLBC_VariantTraits::assign(LLBC_Variant &left, const LLBC_Variant &right)
{
    if (&left == &right)
        return;

    // Execute assignment.
    if (right.IsNil())// Do NIL type data assignment.
    {
        left.BecomeNil();
    }
    else if (right.IsRaw()) // Do RAW type data assignment.
    {
        // If left type is STR/SEQ/DICT type, clean first.
        if (left.IsStr())
            left.CleanStrData();
        else if (left.IsSeq())
            left.CleanSeqData();
        else if (left.IsDict())
            left.CleanDictData();

        // Assignment.
        left.SetType(right.GetType());
        left.GetHolder().raw.uint64Val = right.GetHolder().raw.uint64Val;
    }
    else if (right.IsStr()) // Do STR type data assignment.
    {
        // If left type is RAW/SEQ/DICT type, clean first.
        if (left.IsRaw())
            left.CleanRawData();
        else if (left.IsSeq())
            left.CleanSeqData();
        else if (left.IsDict())
            left.CleanDictData();

        // Assignment.
        left.SetType(right.GetType());
        LLBC_Variant::Holder &lHolder = left.GetHolder();
        const LLBC_Variant::Holder &rHolder = right.GetHolder();
        if (rHolder.obj.str == NULL || rHolder.obj.str->empty())
        {
            if (lHolder.obj.str)
                lHolder.obj.str->clear();
        }
        else
        {
            if (lHolder.obj.str == NULL)
                lHolder.obj.str = LLBC_New1(LLBC_String, *rHolder.obj.str);
            else if (lHolder.obj.str != rHolder.obj.str)
                *lHolder.obj.str = *rHolder.obj.str;
        }
    }
    else if (right.IsSeq()) // Do SEQ type data assignment.
    {
         // If left type is RAW/STR/DICT type, clean first.
        if (left.IsRaw())
            left.CleanRawData();
        else if (left.IsStr())
            left.CleanStrData();
        else if (left.IsDict())
            left.CleanDictData();

        left.SetType(right.GetType());
        LLBC_Variant::Holder &lHolder = left.GetHolder();
        const LLBC_Variant::Holder &rHolder = right.GetHolder();
        if (rHolder.obj.seq == NULL || rHolder.obj.seq->empty())
        {
            if (lHolder.obj.seq)
                lHolder.obj.dict->clear();
        }
        else
        {
            if (lHolder.obj.seq == NULL)
                lHolder.obj.seq = LLBC_New1(LLBC_Variant::Seq, *rHolder.obj.seq);
            else if (lHolder.obj.seq != rHolder.obj.seq)
                *lHolder.obj.seq = *rHolder.obj.seq;
        }
    }
    else if (right.IsDict()) // Do DICT type data assignment.
    {
        // If left type is RAW/STR/SEQ type, clean first.
        if (left.IsRaw())
            left.CleanRawData();
        else if (left.IsStr())
            left.CleanStrData();
        else if (left.IsSeq())
            left.CleanSeqData();

        left.SetType(right.GetType());
        LLBC_Variant::Holder &lHolder = left.GetHolder();
        const LLBC_Variant::Holder &rHolder = right.GetHolder();
        if (rHolder.obj.dict == NULL || rHolder.obj.dict->empty())
        {
            if (lHolder.obj.dict)
                lHolder.obj.dict->clear();
        }
        else
        {
            if (lHolder.obj.dict == NULL)
                lHolder.obj.dict = LLBC_New1(LLBC_Variant::Dict, *rHolder.obj.dict);
            else if (lHolder.obj.dict != rHolder.obj.dict)
                *lHolder.obj.dict = *rHolder.obj.dict;
        }
    }
}

bool LLBC_VariantTraits::eq(const LLBC_Variant &left, const LLBC_Variant &right)
{
    const LLBC_Variant::Holder &lHolder = left.GetHolder();
    const LLBC_Variant::Holder &rHolder = right.GetHolder();
    if (left.IsStr())
    {
        if (!right.IsStr())
            return false;

        if (lHolder.obj.str)
        {
            if (rHolder.obj.str)
                return *lHolder.obj.str == *rHolder.obj.str;
            else
                return lHolder.obj.str->empty();
        }
        else
        {
            if (rHolder.obj.str)
                return rHolder.obj.str->empty();
            else
                return true;
        }
    }
    else if (left.IsSeq())
    {
        if (!right.IsSeq())
            return false;

        const LLBC_Variant::Seq *lSeq = lHolder.obj.seq;
        const LLBC_Variant::Seq *rSeq = rHolder.obj.seq;
        if (lSeq)
        {
            if (rSeq)
                return *lSeq == *rSeq;
            else
                return lSeq->empty();
        }
        else
        {
            if (rSeq)
                return rSeq->empty();
            else
                return true;
        }
    }

    else if (left.IsDict())
    {
        if (!right.IsDict())
            return false;

        const LLBC_Variant::Dict *lDict = lHolder.obj.dict;
        const LLBC_Variant::Dict *rDict = rHolder.obj.dict;
        if (lDict)
        {
            if (rDict)
                return *lDict == *rDict;
            else
                return lDict->empty();
        }
        else
        {
            if (rDict)
                return rDict->empty();
            else
                return true;
        }
    }
    else if (left.IsRaw())
    {
        if (!right.IsRaw())
            return false;

        if ((left.IsDouble() || left.IsFloat()) ||
            (right.IsDouble() || right.IsFloat()))
        {
            return left.AsDouble() == right.AsDouble();
        }

        return (left.GetHolder().raw.uint64Val == 
            right.GetHolder().raw.uint64Val);
    }

    return (left.IsNil() && right.IsNil());
}

bool LLBC_VariantTraits::ne(const LLBC_Variant &left, const LLBC_Variant &right)
{
    return !LLBC_VariantTraits::eq(left, right);
}

bool LLBC_VariantTraits::lt(const LLBC_Variant &left, const LLBC_Variant &right)
{
    if (left.IsDict())
    {
        if (right.IsDict()) // Dict: compare
        {
            const LLBC_Variant::Dict *lDict = left.GetHolder().obj.dict;
            const LLBC_Variant::Dict *rDict = right.GetHolder().obj.dict;

            if (lDict == rDict)
                return false;

            if (lDict == NULL || lDict->empty())
                return rDict && !rDict->empty();
            if (rDict == NULL || rDict->empty())
                return false;

            return *lDict < *rDict;
        }
        else // Seq/Str/Raw/Nil: false
        {
            return false;
        }
    }
    else if (left.IsSeq())
    {
        if (right.IsDict()) // Seq<Dict: true
        {
            return true;
        }
        if (right.IsSeq()) // Seq: exec compare
        {
            const LLBC_Variant::Seq *lSeq = left.GetHolder().obj.seq;
            const LLBC_Variant::Seq *rSeq = right.GetHolder().obj.seq;

            if (lSeq == rSeq)
                return false;

            if (lSeq == NULL || lSeq->empty())
                return rSeq && !rSeq->empty();
            if (rSeq == NULL || rSeq->empty())
                return false;

            return *lSeq < *rSeq;
        }
        else // Str/Raw/Nil: false
        {
            return false;
        }
    }
    else if (left.IsStr())
    {
        if (right.IsDict() ||
            right.IsSeq()) // Dict: true
        {
            return false;
        }
        else if (right.IsStr()) // Str: exec compare
        {
            if (left._holder.obj.str &&
                right._holder.obj.str)
                return *left._holder.obj.str < *right._holder.obj.str;
            else
                return false;
        }
        else // Raw/Nil: false
        {
            return false;
        }
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

            if (left.IsSignedRaw() || right.IsSignedRaw())
                return left.AsInt64() < right.AsInt64();
            else
                return left.AsUInt64() < right.AsUInt64();
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
    LLBC_VariantTraits::assign(ret, left);
    LLBC_VariantTraits::add_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::sub(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret;
    LLBC_VariantTraits::assign(ret, left);
    LLBC_VariantTraits::sub_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::mul(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret;
    LLBC_VariantTraits::assign(ret, left);
    LLBC_VariantTraits::mul_equal(ret, right);
    return ret;
}

LLBC_Variant LLBC_VariantTraits::div(const LLBC_Variant &left, const LLBC_Variant &right)
{
    LLBC_Variant ret;
    LLBC_VariantTraits::assign(ret, left);
    LLBC_VariantTraits::div_equal(ret, right);
    return ret;
}

void LLBC_VariantTraits::add_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    if (left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if (left.IsDict() || right.IsDict())
    {
        if (!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Dict *&lDict = left.GetHolder().obj.dict;
        const LLBC_Variant:: Dict *rDict = right.GetHolder().obj.dict;
        if (lDict == NULL)
        {
            if (rDict == NULL || rDict->empty())
                return;

            lDict = LLBC_New0(LLBC_Variant::Dict);
        }

        if (rDict)
            lDict->insert(rDict->begin(), rDict->end());

        return;
    }
    else if (left.IsSeq() || right.IsSeq())
    {
        if (!left.IsSeq() || !right.IsSeq())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Seq *&lSeq = left.GetHolder().obj.seq;
        const LLBC_Variant:: Seq *rSeq = right.GetHolder().obj.seq;
        if (lSeq == NULL)
        {
            if (rSeq == NULL || rSeq->empty())
                return;

            lSeq = LLBC_New0(LLBC_Variant::Seq);
        }

        if (rSeq)
        {
            for (LLBC_Variant::SeqConstIter rIt = rSeq->begin();
                 rIt != rSeq->end();
                 ++rIt)
            {
                if (std::find(lSeq->begin(), lSeq->end(), *rIt) == lSeq->end())
                    lSeq->push_back(*rIt);
            }
        }

        return;
    }
    else if (left.IsStr() || right.IsStr())
    {
        left = left.AsStr() + right.AsStr();
        return;
    }

    LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_ADD);
}

void LLBC_VariantTraits::sub_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    if (left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if (left.IsDict() || right.IsDict())
    {
        if (!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Dict *&lDict = left.GetHolder().obj.dict;
        const LLBC_Variant::Dict *rDict = right.GetHolder().obj.dict;
        if (lDict == NULL)
        {
            if (rDict == NULL || rDict->empty())
                return;

            lDict = LLBC_New0(LLBC_Variant::Dict);
        }

        if (rDict)
        {
            typedef LLBC_Variant::Dict::const_iterator _It;
            for (_It rIt = rDict->begin(); 
                 rIt != rDict->end() && !lDict->empty(); 
                 rIt++)
            {
                lDict->erase(rIt->first);
            }
        }

        return;
    }
    else if (left.IsSeq() || right.IsSeq())
    {
        if (!left.IsSeq() || !right.IsSeq())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Seq *&lSeq = left.GetHolder().obj.seq;
        const LLBC_Variant::Seq *rSeq = right.GetHolder().obj.seq;
        if (lSeq == NULL)
        {
            if (rSeq == NULL || rSeq->empty())
                return;

            lSeq = LLBC_New0(LLBC_Variant::Seq);
        }

        if (rSeq)
        {
            typedef LLBC_Variant::Seq::const_iterator _It;
            for (_It rIt = rSeq->begin(); 
                 rIt != rSeq->end() && !lSeq->empty(); 
                 rIt++)
            {
                _It lIt;
                while ((lIt = std::find(lSeq->begin(), lSeq->end(), *rIt)) != lSeq->end())
                    lSeq->erase(lIt);
            }
        }

        return;
    }
    else if (left.IsStr() || right.IsStr())
    {
        left = LLBC_FilterOutString(left, right);
        return;
    }

    LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_SUB);
}

void LLBC_VariantTraits::mul_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    if (left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if (left.IsDict() || right.IsDict())
    {
        if (!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Dict *&lDict = left.GetHolder().obj.dict;
        const LLBC_Variant::Dict *rDict = right.GetHolder().obj.dict;
        if ((lDict == NULL || lDict->empty()) ||
            (rDict == NULL || rDict->empty()))
        {
            if (lDict)
                lDict->clear();
            return;
        }

        for (LLBC_Variant::DictIter lIt = lDict->begin();
             lIt != lDict->end();
             )
        {
            if (rDict->find(lIt->first) == rDict->end())
                lIt = lDict->erase(lIt);
            else
                ++lIt;
        }

        return;
    }
    else if (left.IsSeq() || right.IsSeq())
    {
        if (!left.IsSeq() || !right.IsSeq())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Seq *&lSeq = left.GetHolder().obj.seq;
        const LLBC_Variant::Seq *rSeq= right.GetHolder().obj.seq;
        if ((lSeq == NULL || lSeq->empty()) ||
            (rSeq == NULL || rSeq->empty()))
        {
            if (lSeq)
                lSeq->clear();
            return;
        }

        for (LLBC_Variant::SeqIter lIt = lSeq->begin();
             lIt != lSeq->end();
             )
        {
            if (std::find(rSeq->begin(), rSeq->end(), *lIt) == rSeq->end())
                lIt = lSeq->erase(lIt);
            else
                ++lIt;
        }

        return;
    }
    else if (left.IsStr() || right.IsStr())
    {
        if (left.IsStr() && right.IsRaw())
        {
            size_t rawRight = static_cast<size_t>(right.AsUInt32());
            LLBC_Variant::Str *&lStr = left.GetHolder().obj.str;
            if (lStr && !lStr->empty())
                *lStr *= rawRight;
        }
        else
        {
            left.BecomeNil();
        }
        return;
    }

    LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_MUL);
}

void LLBC_VariantTraits::div_equal(LLBC_Variant &left, const LLBC_Variant &right)
{
    if (left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if (left.IsDict() || right.IsDict())
    {
        if (!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Dict *&lDict = left.GetHolder().obj.dict;
        const LLBC_Variant::Dict *rDict = right.GetHolder().obj.dict;
        const bool lDictIsEmpty = !lDict || lDict->empty();
        const bool rDictIsEmpty = !rDict || rDict->empty();
        if (lDictIsEmpty)
        {
            if (rDictIsEmpty)
                return;

            if (!lDict)
                lDict = LLBC_New1(LLBC_Variant::Dict, *rDict);
            else
                *lDict = *rDict;

            return;
        }
        else if (rDictIsEmpty)
        {
            return;
        }

        const LLBC_Variant intersection(left * right);
        const LLBC_Variant::Dict * const &iDict = intersection.GetHolder().obj.dict;
        for (LLBC_Variant::DictIter lIt = lDict->begin();
             lIt != lDict->end();
             )
        {
            if (iDict->find(lIt->first) != iDict->end())
                lIt = lDict->erase(lIt);
            else
                ++lIt;
        }

        for (LLBC_Variant::DictConstIter rIt = rDict->begin();
             rIt != rDict->end();
             ++rIt)
        {
            if (iDict->find(rIt->first) == iDict->end())
                lDict->insert(std::make_pair(rIt->first, rIt->second));
        }

        return;
 
    }
    else if (left.IsSeq() || right.IsSeq())
    {
        if (!left.IsSeq() || !right.IsSeq())
        {
            left.BecomeNil();
            return;
        }

        LLBC_Variant::Seq *&lSeq = left.GetHolder().obj.seq;
        const LLBC_Variant::Seq *rSeq = right.GetHolder().obj.seq;
        const bool lSeqIsEmpty = !lSeq || lSeq->empty();
        const bool rSeqIsEmpty = !rSeq || rSeq->empty();
        if (lSeqIsEmpty)
        {
            if (rSeqIsEmpty)
                return;

            if (!lSeq)
                lSeq = LLBC_New1(LLBC_Variant::Seq, *rSeq);
            else
                *lSeq = *rSeq;

            return;
        }
        else if (rSeqIsEmpty)
        {
            return;
        }

        const LLBC_Variant intersection(left * right);
        const LLBC_Variant::Seq * const &iSeq = intersection.GetHolder().obj.seq;
        for (LLBC_Variant::SeqIter lIt = lSeq->begin();
             lIt != lSeq->end();
             )
        {
            if (std::find(iSeq->begin(), iSeq->end(), *lIt) != iSeq->end())
                lIt = lSeq->erase(lIt);
            else
                ++lIt;
        }

        for (LLBC_Variant::SeqConstIter rIt = rSeq->begin();
             rIt != rSeq->end();
             ++rIt)
        {
            if (std::find(iSeq->begin(), iSeq->end(), *rIt) == iSeq->end())
                lSeq->push_back(*rIt);
        }

        return;
    }
    else if (left.IsStr() || right.IsStr())
    {
        left.BecomeNil();
        return;
    }

    LLBC_VariantArithmetic::Performs(left, right, LLBC_VariantArithmetic::VT_ARITHMETIC_DIV);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
