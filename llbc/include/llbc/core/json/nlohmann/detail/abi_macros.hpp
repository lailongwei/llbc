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

// This file contains all macro definitions affecting or depending on the ABI

#ifndef LLBC_JSON_SKIP_LIBRARY_VERSION_CHECK
    #if defined(LLBC_NLOHMANN_JSON_VERSION_MAJOR) && defined(LLBC_NLOHMANN_JSON_VERSION_MINOR) && defined(LLBC_NLOHMANN_JSON_VERSION_PATCH)
        #if LLBC_NLOHMANN_JSON_VERSION_MAJOR != 3 || LLBC_NLOHMANN_JSON_VERSION_MINOR != 11 || LLBC_NLOHMANN_JSON_VERSION_PATCH != 3
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif

#define LLBC_NLOHMANN_JSON_VERSION_MAJOR 3   // NOLINT(modernize-macro-to-enum)
#define LLBC_NLOHMANN_JSON_VERSION_MINOR 11  // NOLINT(modernize-macro-to-enum)
#define LLBC_NLOHMANN_JSON_VERSION_PATCH 3   // NOLINT(modernize-macro-to-enum)

#ifndef LLBC_JSON_DIAGNOSTICS
    #define LLBC_JSON_DIAGNOSTICS 0
#endif

#ifndef LLBC_JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON 
    #define LLBC_JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON 0
#endif

#if LLBC_JSON_DIAGNOSTICS
    #define LLBC_NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS _diag
#else
    #define LLBC_NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS
#endif

#if LLBC_JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
    #define LLBC_NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON _ldvcmp
#else
    #define LLBC_NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON
#endif

#ifndef LLBC_NLOHMANN_JSON_NAMESPACE_NO_VERSION
    #define LLBC_NLOHMANN_JSON_NAMESPACE_NO_VERSION 0
#endif

// Construct the namespace ABI tags component
#define LLBC_NLOHMANN_JSON_ABI_TAGS_CONCAT_EX(a, b) json_abi ## a ## b
#define LLBC_NLOHMANN_JSON_ABI_TAGS_CONCAT(a, b) \
    LLBC_NLOHMANN_JSON_ABI_TAGS_CONCAT_EX(a, b)

#define LLBC_NLOHMANN_JSON_ABI_TAGS                                       \
    LLBC_NLOHMANN_JSON_ABI_TAGS_CONCAT(                                   \
            LLBC_NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS,                       \
            LLBC_NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON)

// Construct the namespace version component
#define LLBC_NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch) \
    _v ## major ## _ ## minor ## _ ## patch
#define LLBC_NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT(major, minor, patch) \
    LLBC_NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch)

#if LLBC_NLOHMANN_JSON_NAMESPACE_NO_VERSION
#define LLBC_NLOHMANN_JSON_NAMESPACE_VERSION
#else
#define LLBC_NLOHMANN_JSON_NAMESPACE_VERSION                                 \
    LLBC_NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT(LLBC_NLOHMANN_JSON_VERSION_MAJOR, \
                                           LLBC_NLOHMANN_JSON_VERSION_MINOR, \
                                           LLBC_NLOHMANN_JSON_VERSION_PATCH)
#endif

// Combine namespace components
#define LLBC_NLOHMANN_JSON_NAMESPACE_CONCAT_EX(a, b) a ## b
#define LLBC_NLOHMANN_JSON_NAMESPACE_CONCAT(a, b) \
    LLBC_NLOHMANN_JSON_NAMESPACE_CONCAT_EX(a, b)

#ifndef LLBC_NLOHMANN_JSON_NAMESPACE
#define LLBC_NLOHMANN_JSON_NAMESPACE               \
    nlohmann::LLBC_NLOHMANN_JSON_NAMESPACE_CONCAT( \
            LLBC_NLOHMANN_JSON_ABI_TAGS,           \
            LLBC_NLOHMANN_JSON_NAMESPACE_VERSION)
#endif

#ifndef LLBC_NLOHMANN_JSON_NAMESPACE_BEGIN
#define LLBC_NLOHMANN_JSON_NAMESPACE_BEGIN                \
    namespace llbc::nlohmann                              \
    {                                                     \
    inline namespace LLBC_NLOHMANN_JSON_NAMESPACE_CONCAT( \
                LLBC_NLOHMANN_JSON_ABI_TAGS,              \
                LLBC_NLOHMANN_JSON_NAMESPACE_VERSION)     \
    {
#endif

#ifndef LLBC_NLOHMANN_JSON_NAMESPACE_END
#define LLBC_NLOHMANN_JSON_NAMESPACE_END                                     \
    }  /* namespace (inline namespace) NOLINT(readability/namespace) */ \
    }  // namespace ::llbc::nlohmann
#endif

#ifndef LLBC_NLOHMANN_NS
#define LLBC_NLOHMANN_NS ::llbc::nlohmann
#endif
