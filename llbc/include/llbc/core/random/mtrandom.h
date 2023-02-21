//************************************************************************
//  This is a slightly modified version of Equamen mersenne twister.
//
//  Copyright (C) 2009 Chipset
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program. If not, see <http://www.gnu.org/licenses/>.
//************************************************************************

// Original Coyright (c) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura
//
// Functions for MT19937, with initialization improved 2002/2/10.
// Coded by Takuji Nishimura and Makoto Matsumoto.
// This is a faster version by taking Shawn Cokus's optimization,
// Matthe Bellew's simplification, Isaku Wada's real version.
// C++ version by Lyell Haynes (Equamen)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. The names of its contributors may not be used to endorse or promote
//    products derived from this software without specific prior written
//    permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

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

    // generates double floating point numbers in the open interval(0, 1)
    double real()
    {
        return (static_cast<double>(rand()) + .5) * (1. / 4294967296.); // divided by 2^32
    }

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

__LLBC_NS_END
