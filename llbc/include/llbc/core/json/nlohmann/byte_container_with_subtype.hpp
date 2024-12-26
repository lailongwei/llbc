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

#include <cstdint> // uint8_t, uint64_t
#include <tuple> // tie
#include <utility> // move

#include <llbc/core/json/nlohmann/detail/abi_macros.hpp>

LLBC_NLOHMANN_JSON_NAMESPACE_BEGIN

/// @brief an internal type for a backed binary type
/// @sa https://json.nlohmann.me/api/byte_container_with_subtype/
template<typename BinaryType>
class byte_container_with_subtype : public BinaryType
{
  public:
    using container_type = BinaryType;
    using subtype_type = std::uint64_t;

    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/byte_container_with_subtype/
    byte_container_with_subtype() noexcept(noexcept(container_type()))
        : container_type()
    {}

    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/byte_container_with_subtype/
    byte_container_with_subtype(const container_type& b) noexcept(noexcept(container_type(b)))
        : container_type(b)
    {}

    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/byte_container_with_subtype/
    byte_container_with_subtype(container_type&& b) noexcept(noexcept(container_type(std::move(b))))
        : container_type(std::move(b))
    {}

    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/byte_container_with_subtype/
    byte_container_with_subtype(const container_type& b, subtype_type subtype_) noexcept(noexcept(container_type(b)))
        : container_type(b)
        , m_subtype(subtype_)
        , m_has_subtype(true)
    {}

    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/byte_container_with_subtype/
    byte_container_with_subtype(container_type&& b, subtype_type subtype_) noexcept(noexcept(container_type(std::move(b))))
        : container_type(std::move(b))
        , m_subtype(subtype_)
        , m_has_subtype(true)
    {}

    bool operator==(const byte_container_with_subtype& rhs) const
    {
        return std::tie(static_cast<const BinaryType&>(*this), m_subtype, m_has_subtype) ==
               std::tie(static_cast<const BinaryType&>(rhs), rhs.m_subtype, rhs.m_has_subtype);
    }

    bool operator!=(const byte_container_with_subtype& rhs) const
    {
        return !(rhs == *this);
    }

    /// @brief sets the binary subtype
    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/set_subtype/
    void set_subtype(subtype_type subtype_) noexcept
    {
        m_subtype = subtype_;
        m_has_subtype = true;
    }

    /// @brief return the binary subtype
    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/subtype/
    constexpr subtype_type subtype() const noexcept
    {
        return m_has_subtype ? m_subtype : static_cast<subtype_type>(-1);
    }

    /// @brief return whether the value has a subtype
    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/has_subtype/
    constexpr bool has_subtype() const noexcept
    {
        return m_has_subtype;
    }

    /// @brief clears the binary subtype
    /// @sa https://json.nlohmann.me/api/byte_container_with_subtype/clear_subtype/
    void clear_subtype() noexcept
    {
        m_subtype = 0;
        m_has_subtype = false;
    }

  private:
    subtype_type m_subtype = 0;
    bool m_has_subtype = false;
};

LLBC_NLOHMANN_JSON_NAMESPACE_END
