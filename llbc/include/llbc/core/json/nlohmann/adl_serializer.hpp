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

#include <utility>

#include <llbc/core/json/nlohmann/detail/abi_macros.hpp>
#include <llbc/core/json/nlohmann/detail/conversions/from_json.hpp>
#include <llbc/core/json/nlohmann/detail/conversions/to_json.hpp>
#include <llbc/core/json/nlohmann/detail/meta/identity_tag.hpp>

LLBC_NLOHMANN_JSON_NAMESPACE_BEGIN

/// @sa https://json.nlohmann.me/api/adl_serializer/
template<typename ValueType, typename>
struct adl_serializer
{
    /// @brief convert a JSON value to any value type
    /// @sa https://json.nlohmann.me/api/adl_serializer/from_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType && j, TargetType& val) noexcept(
        noexcept(LLBC_NLOHMANN_NS::from_json(std::forward<BasicJsonType>(j), val)))
    -> decltype(LLBC_NLOHMANN_NS::from_json(std::forward<BasicJsonType>(j), val), void())
    {
        LLBC_NLOHMANN_NS::from_json(std::forward<BasicJsonType>(j), val);
    }

    /// @brief convert a JSON value to any value type
    /// @sa https://json.nlohmann.me/api/adl_serializer/from_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType && j) noexcept(
    noexcept(LLBC_NLOHMANN_NS::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {})))
    -> decltype(LLBC_NLOHMANN_NS::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {}))
    {
        return LLBC_NLOHMANN_NS::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {});
    }

    /// @brief convert any value type to a JSON value
    /// @sa https://json.nlohmann.me/api/adl_serializer/to_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto to_json(BasicJsonType& j, TargetType && val) noexcept(
        noexcept(LLBC_NLOHMANN_NS::to_json(j, std::forward<TargetType>(val))))
    -> decltype(LLBC_NLOHMANN_NS::to_json(j, std::forward<TargetType>(val)), void())
    {
        LLBC_NLOHMANN_NS::to_json(j, std::forward<TargetType>(val));
    }
};

LLBC_NLOHMANN_JSON_NAMESPACE_END
