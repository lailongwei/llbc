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

__LLBC_NS_BEGIN

inline LLBC_Random::LLBC_Random(int seed)
: _mtRand(seed != 0 ? seed : static_cast<int>(time(nullptr)))
{
}

inline void LLBC_Random::Seed(int seed)
{
    _mtRand.reset(seed);
}

inline int LLBC_Random::Rand()
{
    return static_cast<int>(_mtRand.rand());
}

inline int LLBC_Random::Rand(int end)
{
    if (LIKELY(end != 0))
    {
        const long long randVal = llabs(static_cast<int>(_mtRand.rand()));
        if (end > 0)
            return static_cast<int>(randVal % end);
        else
            return static_cast<int>(randVal % end + end);
    }
    else
    {
        return 0;
    }
}

inline int LLBC_Random::Rand(int begin, int end)
{
    if (LIKELY(begin != end))
    {
        const long long randVal = llabs(static_cast<int>(_mtRand.rand()));
        if (begin < end)
            return randVal % (end - begin) + begin;
        else
            return randVal % (begin - end) + end;
    }
    else
    {
        return begin;
    }
}

inline double LLBC_Random::RandReal()
{
    return _mtRand.real();
}

inline bool LLBC_Random::BoolJudge()
{
    return _mtRand.rand() % 2 == 1;
}

template <typename _RandomAccessIter>
inline _RandomAccessIter LLBC_Random::Choice(const _RandomAccessIter &begin, const _RandomAccessIter &end)
{
    long diff = static_cast<long>(end - begin);
    if (UNLIKELY(diff <= 0))
        return end;

    return begin + Rand(diff);
}

__LLBC_NS_END
