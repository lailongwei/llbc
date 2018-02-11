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

#include "llbc/core/thread/Guard.h"

#include "llbc/core/random/mtrand.h"
#include "llbc/core/random/Random.h"

__LLBC_INTERNAL_NS_BEGIN

static MTRand_int32 __g_mt_generator;

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

LLBC_SpinLock LLBC_Random::_lock;

uint32 LLBC_Random::RandInt32()
{
    LLBC_LockGuard guard(_lock);
    return static_cast<uint32>(LLBC_INTERNAL_NS __g_mt_generator());
}

uint32 LLBC_Random::RandInt32(uint32 n)
{
    uint32 used = n;
    used |= used >> 1;
    used |= used >> 2;
    used |= used >> 4;
    used |= used >> 8;
    used |= used >> 16;

    uint32 ret;
    do 
    {
        ret = LLBC_Random::RandInt32() & used;
    } while (ret > n);
    
    return ret;
}

uint32 LLBC_Random::RandInt32cmcn(uint32 m, uint32 n)
{
    if (UNLIKELY(m > n))
    {
        LLBC_Swap(m, n);
    }

    return LLBC_Random::RandInt32(n - m) + m;
}

uint32 LLBC_Random::RandInt32cmon(uint32 m, uint32 n)
{
    ASSERT(m != n && "LLBC_Random::RandInt32cmon(): m == n!");
    if (UNLIKELY(m > n))
    {
        LLBC_Swap(m, n);
    }

    return LLBC_Random::RandInt32(n - m - 1) + m;
}

uint32 LLBC_Random::RandInt32omcn(uint32 m, uint32 n)
{
    return LLBC_Random::RandInt32cmon(m, n) + 1;
}

uint32 LLBC_Random::RandInt32omon(uint32 m, uint32 n)
{
    if (UNLIKELY(m > n))
    {
        LLBC_Swap(m, n);
    }

    ASSERT(m - n > 1 && "LLBC_Random::RandInt32omon(): m - n <= 1!");

    // Convert to [m + 1, n).
    m += 1;

    return LLBC_Random::RandInt32(n - m - 1) + m;
}

double LLBC_Random::RandRealc0c1()
{
    return static_cast<double>(LLBC_Random::RandInt32()) * (1.0 / 4294967295.0);
}

double LLBC_Random::RandRealc0cn(double n)
{
    return LLBC_Random::RandRealc0c1() * n;
}

double LLBC_Random::RandRealc0o1()
{
    return static_cast<double>(LLBC_Random::RandInt32()) * (1.0 / 4294967296.0);
}

double LLBC_Random::RandRealc0on(double n)
{
    return LLBC_Random::RandRealc0o1() * n;
}

double LLBC_Random::RandRealo0o1()
{
    return (static_cast<double>(LLBC_Random::RandInt32()) + 0.5) * (1.0 / 4294967296.0);
}

double LLBC_Random::RandRealo0on(double n)
{
    return LLBC_Random::RandRealo0o1() * n;
}

double LLBC_Random::Rand53Real()
{
    uint32 val1 = LLBC_Random::RandInt32() >> 5;
    uint32 val2 = LLBC_Random::RandInt32() >> 6;

    return (val1 * 67108864.0 + val2) * (1.0 / 9007199254740992.0);
}

void LLBC_Random::Seed(unsigned long seed)
{
    LLBC_LockGuard guard(_lock);
    LLBC_INTERNAL_NS __g_mt_generator.seed(seed);
}

void LLBC_Random::Seed(const unsigned long *array, int size)
{
    LLBC_LockGuard guard(_lock);
    LLBC_INTERNAL_NS __g_mt_generator.seed(array, size);
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"
