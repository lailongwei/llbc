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

#include "llbc/common/Macro.h"
#include "llbc/common/OSHeader.h"

__LLBC_NS_BEGIN

/**
 * \brief Template specification judge function.
 */
template <typename Class, template <typename...> class ClassTemplate>
struct LLBC_IsTemplSpec : std::false_type {  };

template <template <typename...> class ClassTemplate, typename... Args>
struct LLBC_IsTemplSpec<ClassTemplate<Args...>, ClassTemplate> : public std::true_type {  };

/**
 * \brief Special Template specification judge function: std::array.
 */
template <typename ArrayClass, template <typename ValueType, size_t N> class ArrayClassTemplate>
struct LLBC_IsSTLArraySpec : std::false_type {  };

template <template <typename ValueType, size_t N> class ArrayClassTemplate, typename ValueType, size_t N>
struct LLBC_IsSTLArraySpec<ArrayClassTemplate<ValueType, N>, ArrayClassTemplate> : public std::true_type {  };

/**
 * \brief Extract given T pure type.
 */
template <typename T>
struct LLBC_ExtractPureType
{
    typedef typename std::remove_const<
                typename std::remove_pointer<
                    typename std::remove_reference<T>::type>::type>::type type;
};


__LLBC_NS_END
