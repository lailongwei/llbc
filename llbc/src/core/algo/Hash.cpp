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

#include "llbc/core/algo/Hash.h"

__LLBC_INTERNAL_NS_BEGIN

static LLBC_NS LLBC_CString __g_HashAlgoEnumStrs[LLBC_NS LLBC_HashAlgo::End + 1] = {
    "BKDR",
    "DJB",
    "SDBM",
    "RS",
    "JS",
    "PJW",
    "ELF",
    "AP",
    "MurmurHash3",

    "Unknown"
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_CString LLBC_HashAlgo::GetEnumStr(int hashAlgo)
{
    return hashAlgo >= Begin && hashAlgo < End ?
        LLBC_INL_NS __g_HashAlgoEnumStrs[hashAlgo] :
            LLBC_INL_NS __g_HashAlgoEnumStrs[LLBC_HashAlgo::End];
}

uint32 LLBC_Hasher::BKDRHash(const void * bytes, size_t size)
{
    constexpr uint32 seed = 131; // 31 131 1313 13131 131313

    uint32 hash = 0;
    const uint8 *u8Buf = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
        hash = hash * seed + u8Buf[i];

    return hash;
}

uint32 LLBC_Hasher::DJBHash(const void *bytes, size_t size)
{
    uint32 hash = 5381;
    const uint8 *u8Bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
    {
        // Equivalent to: hash = hash[i - 1] * 33 + str[i]
        hash += (hash << 5) + u8Bytes[i];
    }

    return hash;
}

uint32 LLBC_Hasher::SDBMHash(const void *bytes, size_t size)
{
    uint32 hash = 0;
    const uint8 *u8Bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
    {
        // Equivalent to: hash = 65599 * hash + (*bytes++);
        hash = u8Bytes[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

uint32 LLBC_Hasher::RSHash(const void *bytes, size_t size)
{
    uint32 a = 63689;
    constexpr uint32 b = 378551;

    uint32 hash = 0;
    const uint8 *u8Bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
    {
        hash = hash * a + (u8Bytes[i]);
        a *= b;
    }

    return hash;
}

uint32 LLBC_Hasher::JSHash(const void *bytes, size_t size)
{
    uint32 hash = 1315423911;
    const uint8 *u8Bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
        hash ^= ((hash << 5) + u8Bytes[i] + (hash >> 2));

    return hash;
}

uint32 LLBC_Hasher::PJWHash(const void *bytes, size_t size)
{
    constexpr uint32 bitsInUInt32 = sizeof(uint32) * 8;
    constexpr uint32 threeQuarters = bitsInUInt32 * 3 / 4;
    constexpr uint32 oneEighth = bitsInUInt32 / 8;
    constexpr uint32 highBits = 0xffffffff << (bitsInUInt32 - oneEighth);

    uint32 test;
    uint32 hash = 0;
    const uint8 *u8Bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
    {
        hash = (hash << oneEighth) + u8Bytes[i];
        if((test = hash & highBits) != 0)
            hash = (hash ^ (test >> threeQuarters)) & (~highBits);
    }

    return hash;
}

uint32 LLBC_Hasher::ELFHash(const void *bytes, size_t size)
{
    uint32 test;
    uint32 hash = 0;
    const uint8 *u8bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
    {
        hash = (hash << 4) + u8bytes[i];
        if((test = hash & 0xF0000000L) != 0)
        {
            hash ^= (test >> 24);
            hash &= ~test;
        }
    }

    return hash;
}

uint32 LLBC_Hasher::APHash(const void *bytes, size_t size)
{
    uint32 hash = 0;
    const uint8 *u8bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; ++i)
    {
        if((i & 1) == 0)
            hash ^= ((hash << 7) ^ u8bytes[i] ^ (hash >> 3));
        else
            hash ^= (~((hash << 11) ^ u8bytes[i] ^ (hash >> 5)));
    }

    return hash;
}

uint32 LLBC_Hasher::MurmurHash3Hash(const void *bytes, size_t size)
{
    constexpr uint32 c1 = 0xcc9e2d51;
    constexpr uint32 c2 = 0x1b873593;
    constexpr uint32 r1 = 15;
    constexpr uint32 r2 = 13;
    constexpr uint32 m = 5;
    constexpr uint32 n = 0xe6546b64;

    uint32 hash = 0;
    const uint8 *u8Bytes = reinterpret_cast<const uint8 *>(bytes);
    for (size_t i = 0; i < size; i += 4)
    {
        uint32 k = 0;
        for (size_t j = 0; j < 4 && i + j < size; ++j)
            k |= u8Bytes[i + j] << (j * 8);

        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = (hash << r2) | (hash >> (32 - r2));
        hash = hash * m + n;
    }

    hash ^= size;
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;

    return hash;
}

__LLBC_NS_END
