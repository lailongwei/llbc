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

#ifndef __LLBC_TEST_CASE_CORE_RANDOM_H__
#define __LLBC_TEST_CASE_CORE_RANDOM_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Core_Random : public LLBC_BaseTestCase
{
public:
    TestCase_Core_Random();
    virtual ~TestCase_Core_Random();

public:
    virtual int Run(int argc, char *argv[]);
};

class mtrandom
{
public:
    mtrandom() : left(1) { init(); }

    explicit mtrandom(size_t seed) : left(1) { init(seed); }

    mtrandom(size_t* init_key, int key_length) : left(1)
    {
        int i = 1, j = 0;
        int k = N > key_length ? N : key_length;
        init();
        for (; k; --k)
        {
            state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL)) + init_key[j] + j; // non linear
            state[i] &= 4294967295UL; // for WORDSIZE > 32 machines
            ++i;
            ++j;
            if (i >= N)
            {
                state[0] = state[N - 1];
                i = 1;
            }
            if (j >= key_length)
                j = 0;
        }

        for (k = N - 1; k; --k)
        {
            state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL)) - i; // non linear
            state[i] &= 4294967295UL; // for WORDSIZE > 32 machines
            ++i;
            if (i >= N)
            {
                state[0] = state[N - 1];
                i = 1;
            }
        }

        state[0] = 2147483648UL; // MSB is 1; assuring non-zero initial array
    }

    void reset(size_t rs)
    {
        init(rs);
        next_state();
    }

    size_t rand()
    {
        size_t y;
        if (0 == --left)
            next_state();
        y = *next++;
        // Tempering
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680UL;
        y ^= (y << 15) & 0xefc60000UL;
        y ^= (y >> 18);
        return y;
    }

    double real() { return (double)rand() / -1L; }

    // generates a random number on [0,1) with 53-bit resolution
    double res53()
    {
        size_t a = rand() >> 5, b = rand() >> 6;
        return (a * 67108864.0 + b) / 9007199254740992.0;
    }

private:
    void init(size_t seed = 19650218UL)
    {
        state[0] = seed & 4294967295UL;
        for (int j = 1; j < N; ++j)
        {
            state[j] = (1812433253UL * (state[j - 1] ^ (state[j - 1] >> 30)) + j);
            // See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
            // In the previous versions, MSBs of the seed affect
            // only MSBs of the array state[].
            // 2002/01/09 modified by Makoto Matsumoto
            state[j] &= 4294967295UL;  // for >32 bit machines
        }
    }

    void next_state()
    {
        size_t* p = state;
        int i;

        for (i = N - M + 1; --i; ++p)
            * p = (p[M] ^ twist(p[0], p[1]));

        for (i = M; --i; ++p)
            * p = (p[M - N] ^ twist(p[0], p[1]));
        *p = p[M - N] ^ twist(p[0], state[0]);
        left = N;
        next = state;
    }

    size_t mixbits(size_t u, size_t v) const
    {
        return (u & 2147483648UL) | (v & 2147483647UL);
    }

    size_t twist(size_t u, size_t v) const
    {
        return ((mixbits(u, v) >> 1) ^ (v & 1UL ? 2567483615UL : 0UL));
    }

    static const int N = 624, M = 397;
    size_t state[N];
    size_t left;
    size_t* next;
};

#endif // !__LLBC_TEST_CASE_CORE_RANDOM_H__
