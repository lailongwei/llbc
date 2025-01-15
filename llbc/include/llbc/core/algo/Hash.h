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

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The hash algorithm encapsulation. 
 */
class LLBC_EXPORT LLBC_HashAlgo
{
public:
    enum ENUM
    {
        Begin,
        BKDR = Begin,
        DJB,
        SDBM,
        RS,
        JS,
        PJW,
        ELF,
        AP,
        MurmurHash3,

        End,

        Default = LLBC_CFG_DEFAULT_HASH_ALGO
    };

    /**
     * Get hash algorithm enum string.
     * @param[in] hashAlgo - the hash algorithm.
     * @return LLBC_CString - the enum string.
     */
    static LLBC_CString GetEnumStr(int hashAlgo);
};

/**
 * \brief The hash algorithms encapsulation.
 */
class LLBC_EXPORT LLBC_Hasher
{
public:
    /**
     * Hash specific bytes.
     * @param bytes - the will hash bytes.
     * @param size  - the will has bytes length.
     * @return uint32 - the hash value.
     */
    template <LLBC_HashAlgo::ENUM HashAlgo = LLBC_HashAlgo::Default>
    static uint32 Hash(const void *bytes, size_t size);

    /**
     * Hash specific bytes.
     * @param[in] hashAlgo  - the hash algorithm type.
     * @param[in] bytes     - the will has bytes.
     * @param[in] size      - the will hash bytes length.
     * @return uint32 - the hash value.
     */
    static uint32 Hash(LLBC_HashAlgo::ENUM hashAlgo, const void *bytes, size_t size);

private:
    /**
     * BKDR hash algorithm.
     */
    static uint32 BKDRHash(const void *bytes, size_t size);

    /**
     * DJB hash algorithm.
     */
    static uint32 DJBHash(const void *bytes, size_t size);

    /**
     * SDBM hash algorithm.
     */
    static uint32 SDBMHash(const void *bytes, size_t size);

    /**
     * RS hash algorithm.
     */
    static uint32 RSHash(const void *bytes, size_t size);

    /**
     * JS hash algorithm.
     */
    static uint32 JSHash(const void *bytes, size_t size);

    /**
     * Peter J. Weinberger hash algorithm.
     */
    static uint32 PJWHash(const void *bytes, size_t size);

    /**
     * ELF hash algorithm.
     */
    static uint32 ELFHash(const void *bytes, size_t size);

    /**
     * AP hash algorithm.
     */
    static uint32 APHash(const void *bytes, size_t size);

    /**
     * MurmurHash3 hash algorithm.
     */
    static uint32 MurmurHash3Hash(const void *bytes, size_t size);
};

/**
 * Hash bytes.
 * @tparam HashAlgo - the hash algorithm.
 * @param[in] bytes - the will hash bytes.
 * @param[in] size  - the byte size.
 * @return - hash code.
 */
template <LLBC_HashAlgo::ENUM HashAlgo = LLBC_HashAlgo::MurmurHash3>
uint32 LLBC_Hash(const void *bytes, size_t size);

__LLBC_NS_END

#include "llbc/core/algo/HashInl.h"
