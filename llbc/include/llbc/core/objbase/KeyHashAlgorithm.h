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

#include "llbc/core/singleton/Singleton.h"

__LLBC_NS_BEGIN

/**
 * \brief The hash algorithm encapsulation. 
 */
class LLBC_EXPORT LLBC_HashAlgo
{
public:
    enum
    {
        Begin,

        BKDR = Begin,
        DJB,
        SDBM,
        RS,
        JS,
        PJ,
        ELF,
        AP,

        End,

        Default = LLBC_CFG_OBJBASE_DICT_KEY_HASH_ALGO
    };
};

/**
 * \brief The hash algorithms encapsulation.
 */
class LLBC_EXPORT __LLBC_Hash
{
public:
    /**
     * @brief Ctor&Dtor
     */
    __LLBC_Hash() = default;
    ~__LLBC_Hash() = default;

public:
    /**
     * Hash specific bytes.
     * @param bytes - the will hash bytes.
     * @param len   - the will has bytes length.
     * @return uint32 - the hash value.
     */
    template <int Algo = LLBC_HashAlgo::Default>
    uint32 operator()(const void *bytes, size_t size);

    /**
     * Hash specific bytes.
     * @param[in] algo  - the key hash algorithm type.
     * @param[in] bytes - the will has bytes.
     * @param[in] len   - the will hash bytes length.
     * @return uint32 - the hash value.
     */
    uint32 operator()(int algo, const void *bytes, size_t size);

private:
    /**
     * Define Hash base class. 
     */
    typedef std::binary_function<const void *, size_t, uint32> _HashBase;

    /**
     * BKDR hash algorithm.
     */
    struct _BKDRHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

    /**
     * DJB hash algorithm.
     */
    struct _DJBHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

    /**
     * SDBM hash algorithm.
     */
    struct _SDBMHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

    /**
     * RS hash algorithm.
     */
    struct _RSHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

    /**
     * JS hash algorithm.
     */
    struct _JSHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

    /**
     * P. J. Weinberger hash algorithm.
     */
    struct _PJHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

    /**
     * ELF hash algorithm.
     */
    struct _ELFHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

    /**
     * AP hash algorithm.
     */
    struct _APHash : public _HashBase
    {
        uint32 operator()(const void *buf, size_t size) const;
    };

private:
    /**
     * Disable assignment. 
     */
    LLBC_DISABLE_ASSIGNMENT(__LLBC_Hash);

private:
    const _BKDRHash _bkdrHash{};
    const _DJBHash _djbHash{};
    const _SDBMHash _sdbmHash{};
    const _RSHash _rsHash{};
    const _JSHash _jsHash{};
    const _PJHash _pjHash{};
    const _ELFHash _elfHash{};
    const _APHash _apHash{};
};

__LLBC_NS_END

#include "llbc/core/objbase/KeyHashAlgorithmInl.h"

// Singleton macro define.
template class LLBC_EXPORT LLBC_NS LLBC_Singleton<LLBC_NS __LLBC_Hash>;
#define LLBC_Hash (*(LLBC_NS LLBC_Singleton<LLBC_NS __LLBC_Hash>::Instance()))


