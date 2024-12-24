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

#include <llbc/core/nlohmann/detail/abi_macros.hpp>
#include <llbc/core/nlohmann/detail/iterators/primitive_iterator.hpp>

LLBC_NLOHMANN_JSON_NAMESPACE_BEGIN

namespace detail
{

/*!
@brief an iterator value

@note This structure could easily be a union, but MSVC currently does not allow
unions members with complex constructors, see https://github.com/nlohmann/json/pull/105.
*/
template<typename BasicJsonType> struct internal_iterator
{
    /// iterator for JSON objects
    typename BasicJsonType::object_t::iterator object_iterator {};
    /// iterator for JSON arrays
    typename BasicJsonType::array_t::iterator array_iterator {};
    /// generic iterator for all other types
    primitive_iterator_t primitive_iterator {};
};

}  // namespace detail
LLBC_NLOHMANN_JSON_NAMESPACE_END
