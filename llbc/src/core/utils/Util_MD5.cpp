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

#include "llbc/core/file/File.h"

#include "llbc/core/utils/Util_Text.h"
#include "llbc/core/utils/Util_MD5.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_NS_BEGIN

#define __LLBC_INL_MD5_ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

LLBC_MD5::MD5GroupDigest::MD5GroupDigest(uint32 a, uint32 b, uint32 c, uint32 d)
: _val{{a, b, c, d}}
{
}

void LLBC_MD5::MD5GroupDigest::Get(uint8 (&digest)[16]) const
{
    memcpy(digest, _val.byteVal, sizeof(digest));
}

void LLBC_MD5::MD5GroupDigest::Get(uint32 &a, uint32 &b, uint32 &c, uint32 &d)
{
    a = _val.sVal.a;
    b = _val.sVal.b;
    c = _val.sVal.c;
    d = _val.sVal.d;
}

LLBC_MD5::MD5GroupDigest &LLBC_MD5::MD5GroupDigest::operator+=(const MD5GroupDigest &right)
{
    _val.sVal.a += right._val.sVal.a;
    _val.sVal.b += right._val.sVal.b;
    _val.sVal.c += right._val.sVal.c;
    _val.sVal.d += right._val.sVal.d;

    return *this;
}

const uint32 LLBC_MD5::MD5Group::Ti[64 + 1] = {
    0,
    (uint32)(4294967296ull * fabs(sin( 1.0))), (uint32)(4294967296ull * fabs(sin( 2.0))), 
    (uint32)(4294967296ull * fabs(sin( 3.0))), (uint32)(4294967296ull * fabs(sin( 4.0))),
    (uint32)(4294967296ull * fabs(sin( 5.0))), (uint32)(4294967296ull * fabs(sin( 6.0))), 
    (uint32)(4294967296ull * fabs(sin( 7.0))), (uint32)(4294967296ull * fabs(sin( 8.0))),
    (uint32)(4294967296ull * fabs(sin( 9.0))), (uint32)(4294967296ull * fabs(sin(10.0))), 
    (uint32)(4294967296ull * fabs(sin(11.0))), (uint32)(4294967296ull * fabs(sin(12.0))),
    (uint32)(4294967296ull * fabs(sin(13.0))), (uint32)(4294967296ull * fabs(sin(14.0))), 
    (uint32)(4294967296ull * fabs(sin(15.0))), (uint32)(4294967296ull * fabs(sin(16.0))),
    (uint32)(4294967296ull * fabs(sin(17.0))), (uint32)(4294967296ull * fabs(sin(18.0))), 
    (uint32)(4294967296ull * fabs(sin(19.0))), (uint32)(4294967296ull * fabs(sin(20.0))),
    (uint32)(4294967296ull * fabs(sin(21.0))), (uint32)(4294967296ull * fabs(sin(22.0))), 
    (uint32)(4294967296ull * fabs(sin(23.0))), (uint32)(4294967296ull * fabs(sin(24.0))),
    (uint32)(4294967296ull * fabs(sin(25.0))), (uint32)(4294967296ull * fabs(sin(26.0))), 
    (uint32)(4294967296ull * fabs(sin(27.0))), (uint32)(4294967296ull * fabs(sin(28.0))),
    (uint32)(4294967296ull * fabs(sin(29.0))), (uint32)(4294967296ull * fabs(sin(30.0))), 
    (uint32)(4294967296ull * fabs(sin(31.0))), (uint32)(4294967296ull * fabs(sin(32.0))),
    (uint32)(4294967296ull * fabs(sin(33.0))), (uint32)(4294967296ull * fabs(sin(34.0))), 
    (uint32)(4294967296ull * fabs(sin(35.0))), (uint32)(4294967296ull * fabs(sin(36.0))),
    (uint32)(4294967296ull * fabs(sin(37.0))), (uint32)(4294967296ull * fabs(sin(38.0))), 
    (uint32)(4294967296ull * fabs(sin(39.0))), (uint32)(4294967296ull * fabs(sin(40.0))),
    (uint32)(4294967296ull * fabs(sin(41.0))), (uint32)(4294967296ull * fabs(sin(42.0))), 
    (uint32)(4294967296ull * fabs(sin(43.0))), (uint32)(4294967296ull * fabs(sin(44.0))),
    (uint32)(4294967296ull * fabs(sin(45.0))), (uint32)(4294967296ull * fabs(sin(46.0))), 
    (uint32)(4294967296ull * fabs(sin(47.0))), (uint32)(4294967296ull * fabs(sin(48.0))),
    (uint32)(4294967296ull * fabs(sin(49.0))), (uint32)(4294967296ull * fabs(sin(50.0))), 
    (uint32)(4294967296ull * fabs(sin(51.0))), (uint32)(4294967296ull * fabs(sin(52.0))),
    (uint32)(4294967296ull * fabs(sin(53.0))), (uint32)(4294967296ull * fabs(sin(54.0))), 
    (uint32)(4294967296ull * fabs(sin(55.0))), (uint32)(4294967296ull * fabs(sin(56.0))),
    (uint32)(4294967296ull * fabs(sin(57.0))), (uint32)(4294967296ull * fabs(sin(58.0))), 
    (uint32)(4294967296ull * fabs(sin(59.0))), (uint32)(4294967296ull * fabs(sin(60.0))),
    (uint32)(4294967296ull * fabs(sin(61.0))), (uint32)(4294967296ull * fabs(sin(62.0))), 
    (uint32)(4294967296ull * fabs(sin(63.0))), (uint32)(4294967296ull * fabs(sin(64.0)))
};

LLBC_MD5::MD5Group::MD5Group(const void *buf, size_t pos)
{
    _buf = LLBC_Malloc(char, GROUP_SIZE);
    memcpy(_buf, reinterpret_cast<const char *>(buf) + pos, GROUP_SIZE);
}

LLBC_MD5::MD5Group::~MD5Group()
{
    LLBC_XFree(_buf);
}

void LLBC_MD5::MD5Group::GenerateDigest(MD5GroupDigest &digest)
{
    uint32 a, b, c, d;
    digest.Get(a, b, c, d);

    // Loop 1:
    FF(a, b, c, d, GetSubGroupValue( 0), S11, Ti[1]);
    FF(d, a, b, c, GetSubGroupValue( 1), S12, Ti[2]);
    FF(c, d, a, b, GetSubGroupValue( 2), S13, Ti[3]);
    FF(b, c, d, a, GetSubGroupValue( 3), S14, Ti[4]);
    FF(a, b, c, d, GetSubGroupValue( 4), S11, Ti[5]);
    FF(d, a, b, c, GetSubGroupValue( 5), S12, Ti[6]);
    FF(c, d, a, b, GetSubGroupValue( 6), S13, Ti[7]);
    FF(b, c, d, a, GetSubGroupValue( 7), S14, Ti[8]);
    FF(a, b, c, d, GetSubGroupValue( 8), S11, Ti[9]);
    FF(d, a, b, c, GetSubGroupValue( 9), S12, Ti[10]);
    FF(c, d, a, b, GetSubGroupValue(10), S13, Ti[11]);
    FF(b, c, d, a, GetSubGroupValue(11), S14, Ti[12]);
    FF(a, b, c, d, GetSubGroupValue(12), S11, Ti[13]);
    FF(d, a, b, c, GetSubGroupValue(13), S12, Ti[14]);
    FF(c, d, a, b, GetSubGroupValue(14), S13, Ti[15]);
    FF(b, c, d, a, GetSubGroupValue(15), S14, Ti[16]);

    // Loop 2:
    GG(a, b, c, d, GetSubGroupValue( 1), S21, Ti[17]);
    GG(d, a, b, c, GetSubGroupValue( 6), S22, Ti[18]);
    GG(c, d, a, b, GetSubGroupValue(11), S23, Ti[19]);
    GG(b, c, d, a, GetSubGroupValue( 0), S24, Ti[20]);
    GG(a, b, c, d, GetSubGroupValue( 5), S21, Ti[21]);
    GG(d, a, b, c, GetSubGroupValue(10), S22, Ti[22]);
    GG(c, d, a, b, GetSubGroupValue(15), S23, Ti[23]);
    GG(b, c, d, a, GetSubGroupValue( 4), S24, Ti[24]);
    GG(a, b, c, d, GetSubGroupValue( 9), S21, Ti[25]);
    GG(d, a, b, c, GetSubGroupValue(14), S22, Ti[26]);
    GG(c, d, a, b, GetSubGroupValue( 3), S23, Ti[27]);
    GG(b, c, d, a, GetSubGroupValue( 8), S24, Ti[28]);
    GG(a, b, c, d, GetSubGroupValue(13), S21, Ti[29]);
    GG(d, a, b, c, GetSubGroupValue( 2), S22, Ti[30]);
    GG(c, d, a, b, GetSubGroupValue( 7), S23, Ti[31]);
    GG(b, c, d, a, GetSubGroupValue(12), S24, Ti[32]);

    // Loop 3:
    HH(a, b, c, d, GetSubGroupValue( 5), S31, Ti[33]);
    HH(d, a, b, c, GetSubGroupValue( 8), S32, Ti[34]);
    HH(c, d, a, b, GetSubGroupValue(11), S33, Ti[35]);
    HH(b, c, d, a, GetSubGroupValue(14), S34, Ti[36]);
    HH(a, b, c, d, GetSubGroupValue( 1), S31, Ti[37]);
    HH(d, a, b, c, GetSubGroupValue( 4), S32, Ti[38]);
    HH(c, d, a, b, GetSubGroupValue( 7), S33, Ti[39]);
    HH(b, c, d, a, GetSubGroupValue(10), S34, Ti[40]);
    HH(a, b, c, d, GetSubGroupValue(13), S31, Ti[41]);
    HH(d, a, b, c, GetSubGroupValue( 0), S32, Ti[42]);
    HH(c, d, a, b, GetSubGroupValue( 3), S33, Ti[43]);
    HH(b, c, d, a, GetSubGroupValue( 6), S34, Ti[44]);
    HH(a, b, c, d, GetSubGroupValue( 9), S31, Ti[45]);
    HH(d, a, b, c, GetSubGroupValue(12), S32, Ti[46]);
    HH(c, d, a, b, GetSubGroupValue(15), S33, Ti[47]);
    HH(b, c, d, a, GetSubGroupValue( 2), S34, Ti[48]);

    // Loop 4:
    II(a, b, c, d, GetSubGroupValue( 0), S41, Ti[49]);
    II(d, a, b, c, GetSubGroupValue( 7), S42, Ti[50]);
    II(c, d, a, b, GetSubGroupValue(14), S43, Ti[51]);
    II(b, c, d, a, GetSubGroupValue( 5), S44, Ti[52]);
    II(a, b, c, d, GetSubGroupValue(12), S41, Ti[53]);
    II(d, a, b, c, GetSubGroupValue( 3), S42, Ti[54]);
    II(c, d, a, b, GetSubGroupValue(10), S43, Ti[55]);
    II(b, c, d, a, GetSubGroupValue( 1), S44, Ti[56]);
    II(a, b, c, d, GetSubGroupValue( 8), S41, Ti[57]);
    II(d, a, b, c, GetSubGroupValue(15), S42, Ti[58]);
    II(c, d, a, b, GetSubGroupValue( 6), S43, Ti[59]);
    II(b, c, d, a, GetSubGroupValue(13), S44, Ti[60]);
    II(a, b, c, d, GetSubGroupValue( 4), S41, Ti[61]);
    II(d, a, b, c, GetSubGroupValue(11), S42, Ti[62]);
    II(c, d, a, b, GetSubGroupValue( 2), S43, Ti[63]);
    II(b, c, d, a, GetSubGroupValue( 9), S44, Ti[64]);

    digest += MD5GroupDigest(a, b, c, d);
}

uint32 LLBC_MD5::MD5Group::GetSubGroupValue(uint32 index)
{
    return *(reinterpret_cast<uint32 *>(_buf + index * SUB_GROUP_SIZE));
}

uint32 LLBC_MD5::MD5Group::F(uint32 x, uint32 y, uint32 z)
{
    return (x & y) | ((~x) & z);
}

uint32 LLBC_MD5::MD5Group::G(uint32 x, uint32 y, uint32 z)
{
    return (x & z) | (y & (~z));
}

uint32 LLBC_MD5::MD5Group::H(uint32 x, uint32 y, uint32 z)
{
    return x ^ y ^ z;
}

uint32 LLBC_MD5::MD5Group::I(uint32 x, uint32 y, uint32 z)
{
    return y ^ (x | (~z));
}

void LLBC_MD5::MD5Group::FF(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti)
{
    a += (F(b, c, d) + Mj + ti);
    a = __LLBC_INL_MD5_ROTATE_LEFT(a, s);
    a += b;
}

void LLBC_MD5::MD5Group::GG(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti)
{
    a += (G(b, c, d) + Mj + ti);
    a = __LLBC_INL_MD5_ROTATE_LEFT(a, s);
    a += b;
}

void LLBC_MD5::MD5Group::HH(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti)
{
    a += (H(b, c, d) + Mj + ti);
    a = __LLBC_INL_MD5_ROTATE_LEFT(a, s);
    a += b;
}

void LLBC_MD5::MD5Group::II(uint32 &a, uint32 b, uint32 c, uint32 d, uint32 Mj, uint32 s, uint32 ti)
{
    a += (I(b, c, d) + Mj + ti);
    a = __LLBC_INL_MD5_ROTATE_LEFT(a, s);
    a += b;
}

const uint32 LLBC_MD5::_chainingValA = 0x67452301;
const uint32 LLBC_MD5::_chainingValB = 0xefcdab89;
const uint32 LLBC_MD5::_chainingValC = 0x98badcfe;
const uint32 LLBC_MD5::_chainingValD = 0x10325476;

LLBC_String LLBC_MD5::Digest(const void *bytes, size_t len)
{
    size_t realGroupCnt = len / MD5Group::GROUP_SIZE;
    uint32 modVal = len % MD5Group::GROUP_SIZE;

    size_t secondaryBufSize;
    unsigned char *secondaryBuf;
    if (bytes == nullptr || len == 0 || modVal == 0)
    {
        secondaryBufSize = MD5Group::GROUP_SIZE;
        secondaryBuf = LLBC_Malloc(unsigned char, secondaryBufSize);
        memset(secondaryBuf, 0, MD5Group::GROUP_SIZE);
        secondaryBuf[0] = 0x80;
    }
    else if (modVal < MD5Group::FILL_LEN)
    {
        secondaryBufSize = MD5Group::GROUP_SIZE;
        secondaryBuf = LLBC_Malloc(unsigned char, secondaryBufSize);
        memset(secondaryBuf, 0, secondaryBufSize);
        memcpy(secondaryBuf, 
            reinterpret_cast<const char *>(bytes) + realGroupCnt * MD5Group::GROUP_SIZE, modVal);
        secondaryBuf[modVal] = 0x80;
    }
    else
    {
        secondaryBufSize = MD5Group::GROUP_SIZE * 2;
        secondaryBuf = LLBC_Malloc(unsigned char, secondaryBufSize);
        memset(secondaryBuf, 0, secondaryBufSize);
        memcpy(secondaryBuf, 
            reinterpret_cast<const char *>(bytes) + realGroupCnt * MD5Group::GROUP_SIZE, modVal);
        secondaryBuf[modVal] = 0x80;
    }

    if (bytes == nullptr || len == 0)
        *reinterpret_cast<unsigned long long *>(
            secondaryBuf + secondaryBufSize - MD5Group::MESSAGE_LEN) = 0;
    else
        *reinterpret_cast<unsigned long long *>(
            secondaryBuf + secondaryBufSize - MD5Group::MESSAGE_LEN) = (len << 3);

    MD5GroupDigest digestObj(_chainingValA, _chainingValB, _chainingValC, _chainingValD);
    for (size_t i = 0; i < realGroupCnt; ++i)
    {
        MD5Group group(bytes, i * MD5Group::GROUP_SIZE);
        group.GenerateDigest(digestObj);
    }

    size_t secondaryGroupCnt = secondaryBufSize / MD5Group::GROUP_SIZE;
    for (size_t i = 0; i < secondaryGroupCnt; ++i)
    {
        MD5Group group(secondaryBuf, i * MD5Group::GROUP_SIZE);
        group.GenerateDigest(digestObj);
    }

    LLBC_XFree(secondaryBuf);

    uint8 digest[16];
    digestObj.Get(digest);

    return LLBC_String().append(reinterpret_cast<const char *>(digest), sizeof(digest));
}

LLBC_String LLBC_MD5::HexDigest(const void *bytes, size_t len)
{
    const auto digest = Digest(bytes, len);
    if (digest.empty())
        return "";

    char hexDigest[33];
    const auto digestPtr = reinterpret_cast<const uint8 *>(digest.data());
    snprintf(hexDigest,
             sizeof(hexDigest),
             "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             digestPtr[0], digestPtr[1], digestPtr[2], digestPtr[3],
             digestPtr[4], digestPtr[5], digestPtr[6], digestPtr[7],
             digestPtr[8], digestPtr[9], digestPtr[10], digestPtr[11],
             digestPtr[12], digestPtr[13], digestPtr[14], digestPtr[15]);

    return hexDigest;
}

LLBC_String LLBC_MD5::FileDigest(const LLBC_String &file)
{
    auto fileCnt = LLBC_File::ReadToEnd(file);
    return LLBC_GetLastError() == LLBC_ERROR_SUCCESS ?
        Digest(fileCnt.data(), fileCnt.size()) : "";
}

LLBC_String LLBC_MD5::FileHexDigest(const LLBC_String &file)
{
    auto fileCnt = LLBC_File::ReadToEnd(file);
    return LLBC_GetLastError() == LLBC_ERROR_SUCCESS ?
        HexDigest(fileCnt.data(), fileCnt.size()) : "";
}

__LLBC_NS_END

#undef __LLBC_INL_MD5_ROTATE_LEFT

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif
