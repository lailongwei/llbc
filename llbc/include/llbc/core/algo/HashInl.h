// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

__LLBC_NS_BEGIN

template <LLBC_HashAlgo::ENUM HashAlgo>
uint32 LLBC_Hasher::Hash(const void *bytes, size_t size)
{
    if constexpr (HashAlgo == LLBC_HashAlgo::BKDR)
        return BKDRHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::DJB)
        return DJBHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::SDBM)
        return SDBMHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::RS)
        return RSHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::JS)
        return JSHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::PJW)
        return PJWHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::ELF)
        return ELFHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::AP)
        return APHash(bytes, size);
    else if constexpr (HashAlgo == LLBC_HashAlgo::MurmurHash3)
        return MurmurHash3Hash(bytes, size);
    else
        static_assert("Invalid hash algorithm");

    return 0;
}

inline uint32 LLBC_Hasher::Hash(LLBC_HashAlgo::ENUM hashAlgo, const void *bytes, size_t size)
{
    if (hashAlgo == LLBC_HashAlgo::BKDR)
        return BKDRHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::DJB)
        return DJBHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::SDBM)
        return SDBMHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::RS)
        return RSHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::JS)
        return JSHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::PJW)
        return PJWHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::ELF)
        return ELFHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::AP)
        return APHash(bytes, size);
    else if (hashAlgo == LLBC_HashAlgo::MurmurHash3)
        return MurmurHash3Hash(bytes, size);
    else
        return 0;
}

template <LLBC_HashAlgo::ENUM HashAlgo>
uint32 LLBC_Hash(const void *bytes, size_t size)
{
    return LLBC_Hasher::Hash<HashAlgo>(bytes, size);
}

__LLBC_NS_END
