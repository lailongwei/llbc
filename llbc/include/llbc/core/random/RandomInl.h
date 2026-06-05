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

inline LLBC_Random::LLBC_Random(uint32 seed)
: _mtRand(seed != 0 ? seed : std::random_device()())
{
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

template <typename _Container, typename _Weights>
std::enable_if_t<(LLBC_IsTemplSpec<std::remove_cv_t<_Weights>, std::vector>::value ||
                  LLBC_IsTemplSpec<std::remove_cv_t<_Weights>, std::list>::value) &&
                 (std::is_integral_v<typename std::remove_cv_t<_Weights>::value_type> ||
                  std::is_floating_point_v<typename std::remove_cv_t<_Weights>::value_type>),
                 std::optional<std::reference_wrapper<const typename _Container::value_type>>>
LLBC_Random::Choice(const _Container &container,
                    const _Weights *weights)
{
    auto out = Choices(container, 1, weights);
    return !out.empty() ? std::make_optional(out[0]) : std::nullopt;
}

template <typename _Container, typename _Weights>
std::enable_if_t<(LLBC_IsTemplSpec<std::remove_cv_t<_Weights>, std::vector>::value ||
                  LLBC_IsTemplSpec<std::remove_cv_t<_Weights>, std::list>::value) &&
                 (std::is_integral_v<typename std::remove_cv_t<_Weights>::value_type> ||
                  std::is_floating_point_v<typename std::remove_cv_t<_Weights>::value_type>),
                 std::vector<std::reference_wrapper<const typename _Container::value_type>>>
LLBC_Random::Choices(const _Container &container,
                     size_t k,
                     const _Weights *weights)
{
    using RawContainer = std::remove_cv_t<_Container>;
    std::vector<std::reference_wrapper<const typename _Container::value_type>> out;

    if (k == 0)
        return out;

    const size_t n = container.size();
    if (n == 0)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return out;
    }

    std::vector<const typename _Container::value_type *> values;
    values.reserve(n);
    if constexpr (LLBC_IsTemplSpec<RawContainer, std::queue>::value)
    {
        class QueueContainerAccessor : private RawContainer
        {
        public:
            static const typename RawContainer::container_type &Get(const RawContainer &queue)
            {
                typename RawContainer::container_type RawContainer::*containerPtr = &QueueContainerAccessor::c;
                return queue.*containerPtr;
            }
        };

        const auto &queueContainer = QueueContainerAccessor::Get(container);
        for (const auto &value : queueContainer)
            values.push_back(std::addressof(value));
    }
    else
    {
        for (const auto &value : container)
            values.push_back(std::addressof(value));
    }

    out.reserve(k > n ? n : k);

    // Unary uniform fast-path: partial Fisher-Yates on pointers, O(min(k,n)).
    if constexpr (!(LLBC_IsTemplSpec<RawContainer, std::map>::value ||
                    LLBC_IsTemplSpec<RawContainer, std::unordered_map>::value))
    {
        if (weights == nullptr)
        {
            for (size_t i = 0, take = k > n ? n : k; i < take; ++i)
            {
                std::swap(values[i], values[i + static_cast<size_t>(Rand(static_cast<int>(n - i)))]);
                out.emplace_back(*values[i]);
            }
            return out;
        }
    }

    std::vector<double> ws(n, 0.0);
    double total = 0.0;
    if (weights != nullptr)
    {
        size_t i = 0;
        for (auto it = std::begin(*weights); i < n && it != std::end(*weights); ++it, ++i)
        {
            const double dw = static_cast<double>(*it);
            if (dw > 0.0) { ws[i] = dw; total += dw; }
        }
    }
    else if constexpr (LLBC_IsTemplSpec<RawContainer, std::map>::value ||
                       LLBC_IsTemplSpec<RawContainer, std::unordered_map>::value)
    {
        static_assert(std::is_arithmetic<typename _Container::mapped_type>::value,
            "LLBC_Random::Choices: pair-like .second must be arithmetic without external weights.");
        for (size_t i = 0; i < n; ++i)
        {
            const double dw = static_cast<double>(values[i]->second);
            if (dw > 0.0) { ws[i] = dw; total += dw; }
        }
    }

    // Roulette-wheel without replacement: O(k*n). total<=0 -> loop skipped, returns empty.
    double remain = total;
    for (size_t i = 0; i < k && remain > 0.0; ++i)
    {
        const double r = RandReal() * remain;
        double acc = 0.0;
        for (size_t j = 0; j < n; ++j)
        {
            if (ws[j] > 0.0 && (acc += ws[j]) > r)
            {
                out.emplace_back(*values[j]);
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
