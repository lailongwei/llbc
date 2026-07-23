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

// Define example factory.
typedef LLBC_NS LLBC_ITestCase *(*__ExampleFactoryFunc)();
typedef std::pair<const char *, __ExampleFactoryFunc> __ExampleFactory;

// The example traits template class encapsulation.
template <int index>
struct __ExampleTraits
{
    // Get example name.
    static const char *Name() { return nullptr; }

    // Get example testcase.
    static LLBC_NS LLBC_ITestCase *Create() { return nullptr; }
};

// Example macros define.
// - Examples begin macro.
#define __EXAMPLES_BEGIN()             \
    enum {                             \
        __EXAMPLE_ID_BEGIN = __LINE__, \
    };

// - Examples end macro.
#define __EXAMPLES_END()                                               \
    enum {                                                             \
        __EXAMPLES_COUNT = __LINE__ - __EXAMPLE_ID_BEGIN,             \
    };                                                                 \
                                                                       \
    static __ExampleFactory __exampleFactories[__EXAMPLES_COUNT] {};   \

// - Example define macro.
#define __DEFINE_EXAMPLE(name, cls)                         \
    enum {                                                  \
        __EXAMPLE_ID_##cls = __LINE__ - __EXAMPLE_ID_BEGIN, \
    };                                                      \
                                                            \
    template <>                                             \
    struct __ExampleTraits<__EXAMPLE_ID_##cls> {            \
        static const char *Name() {                         \
            return name "(" # cls ")";                      \
        }                                                   \
                                                            \
        static LLBC_NS LLBC_ITestCase *Create() {           \
            return new cls;                                 \
        }                                                   \
    };

// - Example name, func fetch macros.
#define __EXAMPLE_NAME(index) __exampleFactories[index].first
#define __EXAMPLE_FUNC(index) __exampleFactories[index].second

// - Example colourful print support.
#define __ExamplePrint(color, fmt, ...) \
    do { \
        int oldColor = LLBC_GetConsoleColor(stdout); \
        LLBC_SetConsoleColor(stdout, color); \
        LLBC_Print(fmt, ##__VA_ARGS__); \
        LLBC_SetConsoleColor(stdout, oldColor); \
    } while (false)

#include "ExampleDef.h"

// The example traits generate loop encapsulation.
template <int index>
struct __ExampleTraitsLoop
{
    static void Generate()
    {
        __ExampleTraitsLoop<index - 1>::Generate();
        __exampleFactories[index - 1] =
            __ExampleFactory(__ExampleTraits<index>::Name(), &__ExampleTraits<index>::Create);
    }
};

template <>
struct __ExampleTraitsLoop<0>
{
    static void Generate() {  }
};
