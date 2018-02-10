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

#include "llbc/common/Config.h"

#if LLBC_CFG_OBJBASE_ENABLED

#include "llbc/objbase/KeyHashAlgorithm.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_String __g_type2Desc[
    LLBC_NS LLBC_KeyHashAlgorithmType::End + 1] =
{
    "SDBM",
    "RS",
    "JS",
    "PJ",
    "ELF",
    "BKDR",
    "DJB",
    "AP",

    "Unknown"
};

static std::map<LLBC_NS LLBC_String, int> __g_desc2Type;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

bool LLBC_KeyHashAlgorithmType::IsLegal(int type)
{
    return (type >= Begin && type < End);
}

const LLBC_String &LLBC_KeyHashAlgorithmType::Type2Str(int type)
{
    return _This::IsLegal(type) ? 
        LLBC_INL_NS __g_type2Desc[type] : 
            LLBC_INL_NS __g_type2Desc[_This::End];
}

int LLBC_KeyHashAlgorithmType::Str2Type(const LLBC_String &algoDesc)
{
    if(UNLIKELY(LLBC_INL_NS __g_desc2Type.empty()))
    {
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::SDBM]] = _This::SDBM;
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::RS]] = _This::RS;
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::JS]] = _This::JS;
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::PJ]] = _This::PJ;
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::ELF]] = _This::ELF;
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::BKDR]] = _This::BKDR;
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::DJB]] = _This::DJB;
        LLBC_INL_NS __g_desc2Type[LLBC_INL_NS __g_type2Desc[_This::AP]] = _This::AP;
    }

    LLBC_String upperAlgoDesc = LLBC_ToUpper(algoDesc.c_str());

    typedef std::map<LLBC_String, int>::const_iterator _Iter;

    _Iter it = LLBC_INL_NS __g_desc2Type.find(upperAlgoDesc);
    return it != LLBC_INL_NS __g_desc2Type.end() ? it->second : _This::End;
}

LLBC_KeyHashAlgorithm::LLBC_KeyHashAlgorithm()
{
    typedef LLBC_KeyHashAlgorithmType _AlgoType;

    m_algos[_AlgoType::SDBM] = new LLBC_KeyHashAlgorithm::SDBMHash;
    m_algos[_AlgoType::RS] = new LLBC_KeyHashAlgorithm::RSHash;
    m_algos[_AlgoType::JS] = new LLBC_KeyHashAlgorithm::JSHash;
    m_algos[_AlgoType::PJ] = new LLBC_KeyHashAlgorithm::PJHash;
    m_algos[_AlgoType::ELF] = new LLBC_KeyHashAlgorithm::ELFHash;
    m_algos[_AlgoType::BKDR] = new LLBC_KeyHashAlgorithm::BKDRHash;
    m_algos[_AlgoType::DJB] = new LLBC_KeyHashAlgorithm::DJBHash;
    m_algos[_AlgoType::AP] = new LLBC_KeyHashAlgorithm::APHash;
}

LLBC_KeyHashAlgorithm::~LLBC_KeyHashAlgorithm()
{
    typedef LLBC_KeyHashAlgorithmType _AlgoType;

    int curType = _AlgoType::Begin;
    for(; curType != _AlgoType::End; curType ++)
    {
        delete m_algos[curType];
    }
}

const LLBC_KeyHashAlgorithm::HashBase *LLBC_KeyHashAlgorithm::GetAlgorithm(int type) const
{
    return LLBC_KeyHashAlgorithmType::IsLegal(type) ? m_algos[type] : NULL;
}

const LLBC_KeyHashAlgorithm::HashBase *LLBC_KeyHashAlgorithm::GetAlgorithm(const LLBC_String &algoDesc) const
{
    return GetAlgorithm(LLBC_KeyHashAlgorithmType::Str2Type(algoDesc));
}

LLBC_KeyHashAlgorithm::SDBMHash::Result_Type 
    LLBC_KeyHashAlgorithm::SDBMHash::operator () (
        LLBC_KeyHashAlgorithm::SDBMHash::Argument1_Type buf, 
        LLBC_KeyHashAlgorithm::SDBMHash::Argument2_Type size) const
{
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size; i ++)
    {
        // Equivalent to: hash = 65599 * hash + (*buf ++);
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    }

    return (hash & 0x7fffffff);
}

LLBC_KeyHashAlgorithm::RSHash::Result_Type 
    LLBC_KeyHashAlgorithm::RSHash::operator ()(
        LLBC_KeyHashAlgorithm::RSHash::Argument1_Type buf, 
        LLBC_KeyHashAlgorithm::RSHash::Argument2_Type size) const
{
    uint32 a = 63689;
    uint32 b = 378551;
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size; i ++)
    {
        hash = hash * a + (str[i]);
        a *= b;
    }

    return (hash & 0x7fffffff);
}

LLBC_KeyHashAlgorithm::JSHash::Result_Type 
    LLBC_KeyHashAlgorithm::JSHash::operator ()(
        LLBC_KeyHashAlgorithm::JSHash::Argument1_Type buf, 
        LLBC_KeyHashAlgorithm::JSHash::Argument2_Type size) const
{
    uint32 hash = 1315423911;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size; i ++)
    {
        hash ^= ((hash << 5) + str[i] + (hash >> 2));
    }

    return (hash & 0x7fffffff);
}

LLBC_KeyHashAlgorithm::PJHash::Result_Type
	LLBC_KeyHashAlgorithm::PJHash::operator ()(
		LLBC_KeyHashAlgorithm::PJHash::Argument1_Type buf,
		LLBC_KeyHashAlgorithm::PJHash::Argument2_Type size) const
{
    static const uint32 bitsInUInt32 = sizeof(uint32) * 8;
    static const uint32 threeQuarters = bitsInUInt32 * 3 / 4;
    static const uint32 oneEighth = bitsInUInt32 / 8;
    static const uint32 highBits = 0xffffffff << (bitsInUInt32 - oneEighth);

    uint32 hash = 0;
    uint32 test = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size;i ++)
    {
        hash = (hash << oneEighth) + str[i];
        if((test = hash & highBits) != 0)
        {
            hash = (hash ^ (test >> threeQuarters)) & (~highBits);
        }
    }

    return (hash & 0x7fffffff);
}

LLBC_KeyHashAlgorithm::ELFHash::Result_Type 
	LLBC_KeyHashAlgorithm::ELFHash::operator ()(
		LLBC_KeyHashAlgorithm::ELFHash::Argument1_Type buf,
		LLBC_KeyHashAlgorithm::ELFHash::Argument2_Type size) const
{
    uint32 x = 0;
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size; i ++)
    {
        hash = (hash << 4) + str[i];
        if((x = hash & 0xF0000000L) != 0)
        {
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }

    return (hash & 0x7fffffff);
}

LLBC_KeyHashAlgorithm::BKDRHash::Result_Type
	LLBC_KeyHashAlgorithm::BKDRHash::operator ()(
		LLBC_KeyHashAlgorithm::BKDRHash::Argument1_Type buf,
		LLBC_KeyHashAlgorithm::BKDRHash::Argument2_Type size) const
{
	uint32 seed = 131;
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size; i ++)
    {
        hash = hash * seed + str[i];
    }

    return (hash & 0x7ffffffff);
}

LLBC_KeyHashAlgorithm::DJBHash::Result_Type
	LLBC_KeyHashAlgorithm::DJBHash::operator ()(
		LLBC_KeyHashAlgorithm::DJBHash::Argument1_Type buf,
		LLBC_KeyHashAlgorithm::DJBHash::Argument2_Type size) const
{
	uint32 hash = 5381;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size; i ++)
    {
        hash += (hash << 5) + str[i];
    }

    return (hash & 0x7fffffff);
}

LLBC_KeyHashAlgorithm::APHash::Result_Type
	LLBC_KeyHashAlgorithm::APHash::operator ()(
		LLBC_KeyHashAlgorithm::APHash::Argument1_Type buf,
		LLBC_KeyHashAlgorithm::APHash::Argument2_Type size) const
{
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for(register size_t i = 0; i < size; i ++)
    {
        if((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ str[i] ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ str[i] ^ (hash >> 5)));
        }
    }

    return (hash & 0x7fffffff);
}

__LLBC_NS_END

#endif // LLBC_CFG_OBJBASE_ENABLED

#include "llbc/common/AfterIncl.h"
