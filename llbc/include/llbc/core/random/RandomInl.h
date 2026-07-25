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
    if (UNLIKELY(end == 0))
        return 0;

    if (end > 0)
        return static_cast<int>(_mtRand() % static_cast<uint64>(end));

    const auto range = static_cast<uint64>(-static_cast<sint64>(end));
    return static_cast<int>(static_cast<sint64>(_mtRand() % range) + end);
}

inline int LLBC_Random::Rand(int begin, int end)
{
    if (UNLIKELY(begin == end))
        return begin;

    const int lower = begin < end ? begin : end;
    const int upper = begin < end ? end : begin;
    const auto range = static_cast<uint64>(
        static_cast<sint64>(upper) - static_cast<sint64>(lower));
    return static_cast<int>(static_cast<sint64>(_mtRand() % range) + lower);
}

template <typename _Weights>
typename std::enable_if<LLBC_IsTemplSpec<_Weights, std::vector>::value ||
                        LLBC_IsTemplSpec<_Weights, std::list>::value ||
                        LLBC_IsSTLArraySpec<_Weights, std::array>::value ||
                        std::is_array<_Weights>::value, int>::type
LLBC_Random::Rand(const _Weights &weights)
{
    uint64 totalWeight = 0;
    for (const auto &weight : weights)
    {
        if (weight <= 0)
            continue;

        const uint64 positiveWeight = static_cast<uint64>(weight);
        if (UNLIKELY(positiveWeight > std::numeric_limits<uint64>::max() - totalWeight))
            return 0;
        totalWeight += positiveWeight;
    }

    if (UNLIKELY(totalWeight == 0))
        return 0;

    int i = 0;
    uint64 currentWeight = 0;
    const uint64 randomWeight =
        ((static_cast<uint64>(_mtRand()) << 32) | _mtRand()) % totalWeight;
    for (const auto &weight : weights)
    {
        if (weight > 0)
            currentWeight += static_cast<uint64>(weight);
        if (randomWeight < currentWeight)
            return i;
        i++;
    }

    return 0;
}

inline double LLBC_Random::RandReal()
{
    return static_cast<double>(_mtRand()) / 4294967296.;
}

inline bool LLBC_Random::BoolJudge()
{
    return _mtRand() % 2 == 1;
}

template <typename _RandomAccessIter>
inline _RandomAccessIter LLBC_Random::Choice(const _RandomAccessIter &begin, const _RandomAccessIter &end)
{
    sint64 diff = static_cast<sint64>(end - begin);
    if (UNLIKELY(diff <= 0 || diff > UINT_MAX))
        return end;

    return begin + _mtRand() % static_cast<uint32>(diff);
}

template <typename _RandomAccessIter>
void LLBC_Random::Shuffle(const _RandomAccessIter &begin, const _RandomAccessIter &end)
{
    // typedef typename std::iterator_traits<_RandomAccessIter>::difference_type _diff_t;
    for (int i = static_cast<int>(end - begin - 1); i > 0; --i)
        std::swap(begin[i], begin[Rand(i + 1)]);
}

__LLBC_NS_END
