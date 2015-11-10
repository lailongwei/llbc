/**
 * @file    VariantTraits.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/11
 * @version 1.0
 *
 * @brief
 */

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
    {
        return;
    }

    left.BecomeNil();
    left.SetType(right.GetType());
    if (right.IsRaw())
    {
        left.GetHolder().raw.uint64Val = right.GetHolder().raw.uint64Val;
    }
    else if (right.IsStr())
    {
        left.GetHolder().str = right.GetHolder().str;
    }
    else if (right.IsDict())
    {
        LLBC_Variant::Holder &lHolder = left.GetHolder();
        const LLBC_Variant::Holder &rHolder = right.GetHolder();
        if (rHolder.dict)
        {
            lHolder.dict = new LLBC_Variant::Dict(*rHolder.dict);
        }
    }
}

bool LLBC_VariantTraits::eq(const LLBC_Variant &left, const LLBC_Variant &right)
{
    if (left.IsStr())
    {
        if (!right.IsStr())
        {
            return false;
        }

        return (left.GetHolder().str == right.GetHolder().str);
    }
    else if (left.IsDict())
    {
        if (!right.IsDict())
        {
            return false;
        }

        const LLBC_Variant::Dict *lDict = left.GetHolder().dict;
        const LLBC_Variant::Dict *rDict = right.GetHolder().dict;
        if (lDict == rDict)
        {
            return true;
        }
        else if (!lDict || !rDict)
        {
            return false;
        }
        else if (lDict->size() != rDict->size())
        {
            return false;
        }

        LLBC_Variant::Dict::const_iterator lIt = lDict->begin();
        LLBC_Variant::Dict::const_iterator rIt = rDict->begin();
        for (; lIt != lDict->end(); lIt ++, rIt ++)
        {
            if (lIt->first != rIt->first || lIt->second != rIt->second)
            {
                return false;
            }
        }

        return true;
    }
    else if (left.IsRaw())
    {
        if (!right.IsRaw())
        {
            return false;
        }

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
    if (left.IsDict() && right.IsDict())
    {
        const LLBC_Variant::Dict *lDict = left.GetHolder().dict;
        const LLBC_Variant::Dict *rDict = right.GetHolder().dict;

        if (lDict == rDict)
        {
            return false;
        }
        else if (lDict == NULL)
        {
            return (!rDict->empty()) ? true : false;
        }
        else if (rDict == NULL)
        {
            return false;
        }

        LLBC_Variant::Dict::const_iterator lIt = lDict->begin();
        LLBC_Variant::Dict::const_iterator rIt = rDict->begin();
        for (; lIt != lDict->end() && rIt != rDict->end(); lIt ++, rIt ++)
        {
            if (lIt->first != rIt->first || lIt->second >= rIt->second)
            {
                return false;
            }
        }

        return(lIt == lDict->end()) ? true : false;
    }
    else if (left.IsStr() && right.IsStr())
    {
        return left.AsStr() < right.AsStr();
    }
    else if (left.IsRaw() && right.IsRaw())
    {
        if ((left.IsDouble() || left.IsFloat()) ||
            (right.IsDouble() || right.IsFloat()))
        {
            return left.AsDouble() < right.AsDouble();
        }

        return left.AsUInt64() < right.AsUInt64();
    }

    return false;
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

        LLBC_Variant::Dict *&lDict = left.GetHolder().dict;
        const LLBC_Variant:: Dict *rDict = right.GetHolder().dict;
        if (!lDict)
        {
            if (!rDict || rDict->empty())
            {
                return;
            }

            lDict = new LLBC_Variant::Dict;
        }

        if (rDict)
        {
            lDict->insert(rDict->begin(), rDict->end());
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

        LLBC_Variant::Dict *&lDict = left.GetHolder().dict;
        const LLBC_Variant::Dict *rDict = right.GetHolder().dict;
        if (!lDict)
        {
            if (!rDict || rDict->empty())
            {
                return;
            }

            lDict = new LLBC_Variant::Dict;
        }

        if (rDict)
        {
            typedef LLBC_Variant::Dict::const_iterator _It;
            for (_It rIt = rDict->begin(); 
                rIt != rDict->end() && !lDict->empty(); 
                rIt ++)
            {
                lDict->erase(rIt->first);
            }
        }

        left.OptimizePerformance();
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
        left.BecomeNil();
        return;
    }
    else if (left.IsStr() || right.IsStr())
    {
        left.BecomeNil();
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
        left.BecomeNil();
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
