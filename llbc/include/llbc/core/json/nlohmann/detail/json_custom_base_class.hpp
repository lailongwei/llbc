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

#include <type_traits> // conditional, is_same

#include <llbc/core/json/nlohmann/detail/abi_macros.hpp>

LLBC_NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail
{

/*!
@brief Default base class of the @ref basic_json class.

So that the correct implementations of the copy / move ctors / assign operators
of @ref basic_json do not require complex case distinctions
(no base class / custom base class used as customization point),
@ref basic_json always has a base class.
By default, this class is used because it is empty and thus has no effect
on the behavior of @ref basic_json.
*/
struct json_default_base {};

template<class T>
using json_base_class = typename std::conditional <
                        std::is_same<T, void>::value,
                        json_default_base,
                        T
                        >::type;

}  // namespace detail
LLBC_NLOHMANN_JSON_NAMESPACE_END
