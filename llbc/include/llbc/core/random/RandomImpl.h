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

#include "mtrand.h"
#ifdef __LLBC_CORE_RANDOM_RANDOM_H__

__LLBC_NS_BEGIN

inline LLBC_Random::LLBC_Random(int seed)
{
    if (seed == 0)
        seed = static_cast<int>(::time(NULL));

    _mtRand.seed(seed);
}

inline void LLBC_Random::Seed(int seed)
{
    _mtRand.seed(seed);
}

inline int LLBC_Random::Rand()
{
    return static_cast<int>(_mtRand());
}

inline int LLBC_Random::Rand(int end)
{
    if (end >= 0)
        return static_cast<int>(_mtRand()) % end;
    else
        return static_cast<int>(_mtRand()) % (-end) + end;
}

inline int LLBC_Random::Rand(int begin, int end)
{
    if (LIKELY(begin <= end))
        return _mtRand() % (end - begin) + begin;
    else
        return _mtRand() % (begin - end) + end;

}

inline double LLBC_Random::RandReal()
{
    return static_cast<double>(_mtRand()) * (1. / 4294967296.); // divided by 2^32
}

inline bool LLBC_Random::BoolJudge()
{
    return _mtRand() % 2 == 1;
}

template <typename _RandomAccessIter>
inline _RandomAccessIter LLBC_Random::Choice(const _RandomAccessIter &begin, const _RandomAccessIter &end)
{
    auto diff = end - begin;
    if (UNLIKELY(diff <= 0))
        return end;
    return begin + Rand(0, diff);
}

__LLBC_NS_END

#endif // __LLBC_CORE_RANDOM_RANDOM_H__
