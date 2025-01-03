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

#ifndef INCLUDE_LLBC_NLOHMANN_JSON_FWD_HPP_
#define INCLUDE_LLBC_NLOHMANN_JSON_FWD_HPP_

#include "llbc/common/Common.h"

#include <cstdint> // int64_t, uint64_t
#include <map> // map
#include <memory> // allocator
#include <string> // string
#include <vector> // vector

#include <llbc/core/json/nlohmann/detail/abi_macros.hpp>

/*!
@brief namespace for Niels Lohmann
@see https://github.com/nlohmann
@since version 1.0.0
*/
LLBC_NLOHMANN_JSON_NAMESPACE_BEGIN

/*!
@brief default JSONSerializer template argument

This serializer ignores the template arguments and uses ADL
([argument-dependent lookup](https://en.cppreference.com/w/cpp/language/adl))
for serialization.
*/
template<typename T = void, typename SFINAE = void>
struct adl_serializer;

/// a class to store JSON values
/// @sa https://json.nlohmann.me/api/basic_json/
template<template<typename U, typename V, typename... Args> class ObjectType =
         std::map,
         template<typename U, typename... Args> class ArrayType = std::vector,
         class StringType = std::string, class BooleanType = bool,
         class NumberIntegerType = std::int64_t,
         class NumberUnsignedType = std::uint64_t,
         class NumberFloatType = double,
         template<typename U> class AllocatorType = std::allocator,
         template<typename T, typename SFINAE = void> class JSONSerializer =
         adl_serializer,
         class BinaryType = std::vector<std::uint8_t>, // cppcheck-suppress syntaxError
         class CustomBaseClass = void>
class basic_json;

/// @brief JSON Pointer defines a string syntax for identifying a specific value within a JSON document
/// @sa https://json.nlohmann.me/api/json_pointer/
template<typename RefStringType>
class json_pointer;

/*!
@brief default specialization
@sa https://json.nlohmann.me/api/json/
*/
using json = basic_json<>;

/// @brief a minimal map-like container that preserves insertion order
/// @sa https://json.nlohmann.me/api/ordered_map/
template<class Key, class T, class IgnoredLess, class Allocator>
struct ordered_map;

/// @brief specialization that maintains the insertion order of object keys
/// @sa https://json.nlohmann.me/api/ordered_json/
using ordered_json = basic_json<LLBC_NLOHMANN_NS::ordered_map>;

LLBC_NLOHMANN_JSON_NAMESPACE_END

__LLBC_NS_BEGIN

// nlohmann json
using LLBCJson = LLBC_NLOHMANN_NS::json;
// nlohmann ordered json
using LLBCOrderedJson = nlohmann::ordered_json;

__LLBC_NS_END

#endif  // INCLUDE_NLOHMANN_JSON_FWD_HPP_
