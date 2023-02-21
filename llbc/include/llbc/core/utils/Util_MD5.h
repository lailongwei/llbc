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
 * \brief The MD5 algorithm class encapsulation.
 */
class LLBC_EXPORT LLBC_MD5
{
public:
    /**
     * \brief Embedded class, use to encapsulation MD5GroupDigest class.
     */
    class LLBC_EXPORT MD5GroupDigest
    {
    public:
        /**
         * Parameter constructor.
         */
        MD5GroupDigest(uint32 a, uint32 b, uint32 c, uint32 d);

        /**
         * Get / Set a value.
         */
        uint32 GetA() const;
        void SetA(uint32 a);

        /**
         * Get / Set b value.
         */
        uint32 GetB() const;
        void SetB(uint32 b);

        /**
         * Get / Set c value.
         */
        uint32 GetC() const;
        void SetC(uint32 c);

        /**
         * Get / Set d value.
         */
        uint32 GetD() const;
        void SetD(uint32 d);

        /**
         * Set group all values info.
         */
        void SetInfo(uint32 a, uint32 b, uint32 c, uint32 d);

        /**
         * Convert group digest to string format.
         */
        LLBC_String ToString() const;

        /**
         * += operation overlapped function.
         */
        MD5GroupDigest &operator +=(const MD5GroupDigest &right);

    private:
        union
        {
            struct
            {
                uint32 a;
                uint32 b;
                uint32 c;
                uint32 d;
            } sVal;

            unsigned char byteVal[16];
        } _val;
    };

    /**
     * \brief Embedded class, Use to encapsulation MD5Group class.
     */
    class LLBC_EXPORT MD5Group
    {
    public:
        MD5Group(const void *buf, size_t pos);
        ~MD5Group();

        /**
         * Generage digest.
         * @param[out] digest - store location for group digest.
         */
        void GenerateDigest(MD5GroupDigest &digest);

        // Some value enumeration.
        enum
        {
            SUB_GROUP_SIZE = 32 / 8,
            GROUP_SIZE = 512 / 8,
            MESSAGE_LEN = 64 / 8,
            FILL_LEN = 448 / 8
        };

        // Left shift mark.
        enum
        {
            S11 = 7,
            S12 = 12,
            S13 = 17,
            S14 = 22,
            S21 = 5,
            S22 = 9,
            S23 = 14,
            S24 = 20,
            S31 = 4,
            S32 = 11,
            S33 = 16,
            S34 = 23,
            S41 = 6,
            S42 = 10,
            S43 = 15,
            S44 = 21
        };

    private:
        /**
         * Get sub group value.
         * @param[in] index - the sub group index.
         * @return uint32 - sub group value(32 bit).
         */
        uint32 GetSubGroupValue(uint32 index);

        /**
         * Internal functions, use to generate digest value.
         */
        uint32 F(uint32 x, uint32 y, uint32 z);
        uint32 G(uint32 x, uint32 y, uint32 z);
        uint32 H(uint32 x, uint32 y, uint32 z);
        uint32 I(uint32 x, uint32 y, uint32 z);

        /**
         * Internal functions, use to generate digest value.
         */
        void FF(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti);
        void GG(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti);
        void HH(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti);
        void II(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti);

    private:
        char *_buf;
        static const uint32 Ti[GROUP_SIZE + 1];
    };

public:
    /**
     * Generate the specified file MD5 string.
     * @param[in] file - file name.
     * @return LLBC_String - md5 value.
     */
    static LLBC_String MD5_File(const LLBC_String &file);

    /**
     * Generate the specified string MD5 string.
     * @param[in] str - string value.
     * @return LLBC_String - md5 value.
     */
    static LLBC_String MD5_String(const char *str);
    /**
     * Generate the specified buffer MD5 string.
     * @param[in] buf - buffer pointer.
     * @param[in] len - buffer length, in bytes.
     * @return LLBC_String - md5 value.
     */
    static LLBC_String MD5_Buffer(const void *buf, size_t len);

    /**
     * Generate the specified buffer MD5 digest value.
     * @param[in] buf - buffer pointer.
     * @param[in] len - buffer lenght, in bytes.
     * @param[out] digest - the md5 digest value.
     * @return int - return 0 if success, otherwise return -1.
     */
    static int GenerateBufferDigest(const void *buf, size_t len, MD5GroupDigest &digest);

private:
    static const uint32 _chainingValA;
    static const uint32 _chainingValB;
    static const uint32 _chainingValC;
    static const uint32 _chainingValD;
};

__LLBC_NS_END
