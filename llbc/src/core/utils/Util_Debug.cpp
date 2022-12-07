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

#include "llbc/core/os/OS_Time.h"
#include "llbc/core/time/Time.h"
#include "llbc/core/utils/Util_Debug.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

__LLBC_INTERNAL_NS_BEGIN

static const char *__g_hexTable[256] = 
{
    "00 ",  "01 ",  "02 ",  "03 ",  "04 ",  "05 ",  "06 ",  "07 ",  "08 ",  "09 ",  "0a ",  "0b ",  "0c ",  "0d ",  "0e ",  "0f ",
    "10 ",  "11 ",  "12 ",  "13 ",  "14 ",  "15 ",  "16 ",  "17 ",  "18 ",  "19 ",  "1a ",  "1b ",  "1c ",  "1d ",  "1e ",  "1f ",
    "20 ",  "21 ",  "22 ",  "23 ",  "24 ",  "25 ",  "26 ",  "27 ",  "28 ",  "29 ",  "2a ",  "2b ",  "2c ",  "2d ",  "2e ",  "2f ",
    "30 ",  "31 ",  "32 ",  "33 ",  "34 ",  "35 ",  "36 ",  "37 ",  "38 ",  "39 ",  "3a ",  "3b ",  "3c ",  "3d ",  "3e ",  "3f ",
    "40 ",  "41 ",  "42 ",  "43 ",  "44 ",  "45 ",  "46 ",  "47 ",  "48 ",  "49 ",  "4a ",  "4b ",  "4c ",  "4d ",  "4e ",  "4f ",
    "50 ",  "51 ",  "52 ",  "53 ",  "54 ",  "55 ",  "56 ",  "57 ",  "58 ",  "59 ",  "5a ",  "5b ",  "5c ",  "5d ",  "5e ",  "5f ",
    "60 ",  "61 ",  "62 ",  "63 ",  "64 ",  "65 ",  "66 ",  "67 ",  "68 ",  "69 ",  "6a ",  "6b ",  "6c ",  "6d ",  "6e ",  "6f ",
    "70 ",  "71 ",  "72 ",  "73 ",  "74 ",  "75 ",  "76 ",  "77 ",  "78 ",  "79 ",  "7a ",  "7b ",  "7c ",  "7d ",  "7e ",  "7f ",
    "80 ",  "81 ",  "82 ",  "83 ",  "84 ",  "85 ",  "86 ",  "87 ",  "88 ",  "89 ",  "8a ",  "8b ",  "8c ",  "8d ",  "8e ",  "8f ",
    "90 ",  "91 ",  "92 ",  "93 ",  "94 ",  "95 ",  "96 ",  "97 ",  "98 ",  "99 ",  "9a ",  "9b ",  "9c ",  "9d ",  "9e ",  "9f ",
    "a0 ",  "a1 ",  "a2 ",  "a3 ",  "a4 ",  "a5 ",  "a6 ",  "a7 ",  "a8 ",  "a9 ",  "aa ",  "ab ",  "ac ",  "ad ",  "ae ",  "af ",
    "b0 ",  "b1 ",  "b2 ",  "b3 ",  "b4 ",  "b5 ",  "b6 ",  "b7 ",  "b8 ",  "b9 ",  "ba ",  "bb ",  "bc ",  "bd ",  "be ",  "bf ",
    "c0 ",  "c1 ",  "c2 ",  "c3 ",  "c4 ",  "c5 ",  "c6 ",  "c7 ",  "c8 ",  "c9 ",  "ca ",  "cb ",  "cc ",  "cd ",  "ce ",  "cf ",
    "d0 ",  "d1 ",  "d2 ",  "d3 ",  "d4 ",  "d5 ",  "d6 ",  "d7 ",  "d8 ",  "d9 ",  "da ",  "db ",  "dc ",  "dd ",  "de ",  "df ",
    "e0 ",  "e1 ",  "e2 ",  "e3 ",  "e4 ",  "e5 ",  "e6 ",  "e7 ",  "e8 ",  "e9 ",  "ea ",  "eb ",  "ec ",  "ed ",  "ee ",  "ef ",
    "f0 ",  "f1 ",  "f2 ",  "f3 ",  "f4 ",  "f5 ",  "f6 ",  "f7 ",  "f8 ",  "f9 ",  "fa ",  "fb ",  "fc ",  "fd ",  "fe ",  "ff "
};

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

std::string LLBC_Byte2Hex(const void *buf, size_t len, uint32 lineWidth)
{
    std::string ret;
    if (!buf || len == 0)
    {
        return ret;
    }

    if (UNLIKELY(lineWidth == 0))
    {
        lineWidth = 16;
    }

#if LLBC_TARGET_PLATFORM_NON_WIN32
    ret.reserve(len * 3 + len / lineWidth * 1);
#else
    ret.reserve(len * 3 + len / lineWidth * 2);
#endif

    for (size_t i = 0; i < len - 1; ++i)
    {
        if (i != 0 && i % lineWidth == 0)
        {
#if LLBC_TARGET_PLATFORM_WIN32
            ret += '\r';
#endif
            ret += '\n';
        }

        ret += LLBC_INTERNAL_NS __g_hexTable[*((uint8 *)buf + i)];
    }

    if (len != 1 && (len - 1) % lineWidth == 0)
    {
#if LLBC_TARGET_PLATFORM_WIN32
        ret += '\r';
#endif
        ret += '\n';
    }

    ret += LLBC_INTERNAL_NS __g_hexTable[*((uint8 *)buf + len - 1)];

    return ret;
}

uint64 LLBC_CPUTime::_freqPerSecond = 0;

LLBC_CPUTime LLBC_CPUTime::Current()
{
    return LLBC_CPUTime(LLBC_RdTsc());
}

LLBC_String LLBC_CPUTime::ToString() const
{
    LLBC_String info;
    #if LLBC_SUPPORT_RDTSC
    info.append_format("%.03f", _cpuCount * 1000000ull / _freqPerSecond / 1000.0);
    #else // Not supp rdtsc
    info.append_format("%.03f", _cpuCount / 1000.0);
    #endif // Supp rdtsc

    return info;
}

void LLBC_CPUTime::InitFrequency()
{
#if LLBC_SUPPORT_RDTSC
    _freqPerSecond = LLBC_GetCpuCounterFrequency();
#else // Not supp rdtsc
    _freqPerSecond = LLBC_INFINITE;
#endif // Supp rdtsc
}

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(default:4996)
#endif

__LLBC_NS_END

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_CPUTime &cpuTime)
{
    o << cpuTime.ToString();
    return o;
}
