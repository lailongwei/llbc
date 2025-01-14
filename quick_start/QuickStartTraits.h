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

#include "llbc.h"

// Define quick start factory.
typedef LLBC_NS LLBC_ITestCase *(*__QuickStartFactoryFunc)();
typedef std::pair<const char *, __QuickStartFactoryFunc> __QuickStartFactory;

// The quick start traits template class encapsulation.
template <int index>
struct __QuickStartTraits
{
    // Get quick start name.
    static const char *Name() { return nullptr; }

    // Get quick start testcase.
    static LLBC_NS LLBC_ITestCase *Create() { return nullptr; }
};

// Quick start macros define.
// - Quick starts begin macro.
#define __QUICK_STARTS_BEGIN()             \
    enum {                                 \
        __QUICK_START_ID_BEGIN = __LINE__, \
    };

// - Quick starts end macro.
#define __QUICK_STARTS_END()                                                   \
    enum {                                                                     \
        __QUICK_STARTS_COUNT = __LINE__ - __QUICK_START_ID_BEGIN,              \
    };                                                                         \
                                                                               \
    static __QuickStartFactory __quickStartFactories[__QUICK_STARTS_COUNT] {}; \

// - Quick start define macro.
#define __DEFINE_QUICK_START(name, cls)                             \
    enum {                                                          \
        __QUICK_START_ID_##cls = __LINE__ - __QUICK_START_ID_BEGIN, \
    };                                                              \
                                                                    \
    template <>                                                     \
    struct __QuickStartTraits<__QUICK_START_ID_##cls> {             \
        static const char *Name() {                                 \
            return name "(" # cls ")";                              \
        }                                                           \
                                                                    \
        static LLBC_NS LLBC_ITestCase *Create() {                   \
            return new cls;                                         \
        }                                                           \
    };

// - Quick start name, func fetch macros.
#define __QUICK_START_NAME(index) __quickStartFactories[index].first
#define __QUICK_START_FUNC(index) __quickStartFactories[index].second

// - Quick start colourful print support.
#define __QuickStartPrint(color, fmt, ...) \
    do { \
        int oldColor = LLBC_GetConsoleColor(stdout); \
        LLBC_SetConsoleColor(stdout, color); \
        LLBC_Print(fmt, ##__VA_ARGS__); \
        LLBC_SetConsoleColor(stdout, oldColor); \
    } while (false)

#include "QuickStartDef.h"

// The quick start traits generate loop encapsulation.
template <int index>
struct __QuickStartTraitsLoop
{
    static void Generate()
    {
        __QuickStartTraitsLoop<index - 1>::Generate();
        __quickStartFactories[index - 1] =
            __QuickStartFactory(__QuickStartTraits<index>::Name(), &__QuickStartTraits<index>::Create);
    }
};

template <>
struct __QuickStartTraitsLoop<0>
{
    static void Generate() {  }
};

