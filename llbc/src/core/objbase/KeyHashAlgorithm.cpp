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

#include "llbc/common/Config.h"

#include "llbc/core/utils/Util_Text.h"

#include "llbc/core/objbase/KeyHashAlgorithm.h"

__LLBC_NS_BEGIN

uint32 __LLBC_Hash::operator()(int algo, const void *bytes, size_t size)
{
    if (algo == LLBC_HashAlgo::BKDR)
        return _bkdrHash(bytes, size);
    else if (algo == LLBC_HashAlgo::DJB)
        return _djbHash(bytes, size);
    else if (algo == LLBC_HashAlgo::SDBM)
        return _sdbmHash(bytes, size);
    else if (algo == LLBC_HashAlgo::RS)
        return _rsHash(bytes, size);
    else if (algo == LLBC_HashAlgo::JS)
        return _jsHash(bytes, size);
    else if (algo == LLBC_HashAlgo::PJ)
        return _pjHash(bytes, size);
    else if (algo == LLBC_HashAlgo::ELF)
        return _elfHash(bytes, size);
    else if (algo == LLBC_HashAlgo::AP)
        return _apHash(bytes, size);
    else
        return 0;
}

uint32 __LLBC_Hash::_BKDRHash::operator()(const void * buf, size_t size) const
{
    uint32 seed = 131;
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
    {
        hash = hash * seed + str[i];
    }

    return (hash & 0x7ffffffff);
}

uint32 __LLBC_Hash::_DJBHash::operator()(const void *buf, size_t size) const
{
    uint32 hash = 5381;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
    {
        // Equivalent to: hash = hash[i - 1] * 33 + str[i]
        hash += (hash << 5) + str[i];
    }

    return (hash & 0x7fffffff);
}

uint32 __LLBC_Hash::_SDBMHash::operator() (const void *buf, size_t size) const
{
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
    {
        // Equivalent to: hash = 65599 * hash + (*buf ++);
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    }

    return (hash & 0x7fffffff);
}

uint32 __LLBC_Hash::_RSHash::operator()(const void *buf, size_t size) const
{
    uint32 a = 63689;
    uint32 b = 378551;
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
    {
        hash = hash * a + (str[i]);
        a *= b;
    }

    return (hash & 0x7fffffff);
}

uint32 __LLBC_Hash::_JSHash::operator()(const void *buf, size_t size) const
{
    uint32 hash = 1315423911;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
    {
        hash ^= ((hash << 5) + str[i] + (hash >> 2));
    }

    return (hash & 0x7fffffff);
}

uint32 __LLBC_Hash::_PJHash::operator()(const void *buf, size_t size) const
{
    static const uint32 bitsInUInt32 = sizeof(uint32) * 8;
    static const uint32 threeQuarters = bitsInUInt32 * 3 / 4;
    static const uint32 oneEighth = bitsInUInt32 / 8;
    static const uint32 highBits = 0xffffffff << (bitsInUInt32 - oneEighth);

    uint32 hash = 0;
    uint32 test = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
    {
        hash = (hash << oneEighth) + str[i];
        if((test = hash & highBits) != 0)
        {
            hash = (hash ^ (test >> threeQuarters)) & (~highBits);
        }
    }

    return (hash & 0x7fffffff);
}

uint32 __LLBC_Hash::_ELFHash::operator()(const void *buf, size_t size) const
{
    uint32 x = 0;
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
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

uint32 __LLBC_Hash::_APHash::operator()(const void *buf, size_t size) const
{
    uint32 hash = 0;

    const uint8 *str = reinterpret_cast<const uint8 *>(buf);
    for (size_t i = 0; i < size; ++i)
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
