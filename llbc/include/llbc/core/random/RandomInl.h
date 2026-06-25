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
    if (LIKELY(end != 0))
    {
        if (end > 0)
            return _mtRand() % end;
        else
            return _mtRand() % -end + end;
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
        if (begin < end)
            return _mtRand() % (end - begin) + begin;
        else
            return _mtRand() % (begin - end) + end;
    }
    else
    {
        return begin;
    }
}

inline double LLBC_Random::RandReal()
{
    return static_cast<double>(_mtRand()) / 4294967296.;
}

inline bool LLBC_Random::BoolJudge()
{
    return _mtRand() % 2 == 1;
}

template <typename _InputIt>
_InputIt LLBC_Random::Choice(_InputIt first, _InputIt last)
{
    const auto n = std::distance(first, last);
    if (n <= 0)
        return last;

    std::advance(first, Rand(static_cast<int>(n)));
    return first;
}

template <typename _InputIt, typename _WeightIt>
_InputIt LLBC_Random::WeightedChoice(_InputIt first, _InputIt last,
                                     _WeightIt wfirst, _WeightIt wlast)
{
    // Sum positive weights (missing -> 0; excess -> ignored).
    double total = 0.0;
    auto it = first;
    auto wit = wfirst;
    for (; it != last && wit != wlast; ++it, ++wit)
    {
        const double dw = static_cast<double>(*wit);
        if (dw > 0.0) total += dw;
    }

    if (total <= 0.0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return last;
    }

    // Roulette-wheel selection.
    const double r = RandReal() * total;
    double acc = 0.0;
    for (it = first, wit = wfirst; it != last && wit != wlast; ++it, ++wit)
    {
        const double dw = static_cast<double>(*wit);
        if (dw > 0.0 && (acc += dw) > r)
            return it;
    }

    return last;
}

template <typename _PopIt, typename _OutIt, typename _Distance>
_OutIt LLBC_Random::Sample(_PopIt first, _PopIt last,
                           _OutIt out, _Distance n)
{
    if (n <= 0)
        return out;
    if (first == last)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return out;
    }

    using IterCat = typename std::iterator_traits<_PopIt>::iterator_category;
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, IterCat>)
    {
        // Partial Fisher-Yates on iterator buffer, O(min(n, distance)).
        const size_t total = static_cast<size_t>(last - first);
        std::vector<_PopIt> buf;
        buf.reserve(total);
        for (auto it = first; it != last; ++it)
            buf.push_back(it);

        const size_t take = static_cast<size_t>(n) > total ? total : static_cast<size_t>(n);
        for (size_t i = 0; i < take; ++i)
        {
            std::swap(buf[i], buf[i + static_cast<size_t>(Rand(static_cast<int>(total - i)))]);
            *out++ = *buf[i];
        }
    }
    else
    {
        // Reservoir sampling (Algorithm R), O(distance).
        const size_t k = static_cast<size_t>(n);
        std::vector<_PopIt> reservoir;
        reservoir.reserve(k);

        size_t i = 0;
        for (auto it = first; it != last; ++it, ++i)
        {
            if (i < k)
            {
                reservoir.push_back(it);
            }
            else
            {
                const size_t j = static_cast<size_t>(Rand(static_cast<int>(i + 1)));
                if (j < k) reservoir[j] = it;
            }
        }

        for (auto &it : reservoir)
            *out++ = *it;
    }
    return out;
}

template <typename _PopIt, typename _WeightIt,
          typename _OutIt, typename _Distance>
_OutIt LLBC_Random::WeightedSample(_PopIt first, _PopIt last,
                                   _WeightIt wfirst, _WeightIt wlast,
                                   _OutIt out, _Distance n)
{
    if (n <= 0)
        return out;

    // Buffer iterators and aligned weights (NaN / <=0 -> 0).
    // Empty range or all-zero weights -> total == 0.0 -> handled below.
    std::vector<_PopIt> pop;
    std::vector<double> ws;
    double total = 0.0;
    auto wit = wfirst;
    for (auto it = first; it != last; ++it)
    {
        double dw = 0.0;
        if (wit != wlast)
        {
            dw = static_cast<double>(*wit++);
            if (!(dw > 0.0)) dw = 0.0;
        }
        pop.push_back(it);
        ws.push_back(dw);
        total += dw;
    }

    if (total <= 0.0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return out;
    }

    // Roulette-wheel without replacement: O(n * popLen).
    const size_t popLen = pop.size();
    const size_t want = static_cast<size_t>(n);
    double remain = total;
    for (size_t picks = 0; picks < want && remain > 0.0; ++picks)
    {
        const double r = RandReal() * remain;
        double acc = 0.0;
        for (size_t j = 0; j < popLen; ++j)
        {
            if (ws[j] > 0.0 && (acc += ws[j]) > r)
            {
                *out++ = *pop[j];
                remain -= ws[j];
                ws[j] = 0.0;
                break;
            }
        }
    }
    return out;
}

template <typename _RandomAccessIter>
void LLBC_Random::Shuffle(const _RandomAccessIter &begin, const _RandomAccessIter &end)
{
    // typedef typename std::iterator_traits<_RandomAccessIter>::difference_type _diff_t;
    for (int i = static_cast<int>(end - begin - 1); i > 0; --i)
        std::swap(begin[i], begin[Rand(i + 1)]);
}

__LLBC_NS_END
