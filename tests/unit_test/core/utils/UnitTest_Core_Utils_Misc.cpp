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

#include <llbc.h>
using namespace llbc;

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/utils/Util_Misc.cpp

// StartArgs models a process command line: argv[0] is the executable path, every
// remaining token is positional, and non-empty keys in key=value tokens form a
// lookup map.
TEST(MiscUtilTest, StartArgsParsesPositionalAndNamedArguments)
{
    char module[] = "/opt/llbc/demo";
    char positional[] = "input.txt";
    char mode[] = "mode=debug";
    char emptyValue[] = "empty=";
    char compoundValue[] = "filter=a=b";
    char noKey[] = "=ignored";
    char *argv[] = {module, positional, mode, emptyValue, compoundValue, noKey};

    LLBC_StartArgs args;
    EXPECT_FALSE(args.IsParsed());
    EXPECT_EQ(args.Parse(static_cast<int>(std::size(argv)), argv), LLBC_OK);
    EXPECT_TRUE(args.IsParsed());

    EXPECT_EQ(args.GetModuleFilePath(), module);
    EXPECT_EQ(args.GetArgumentsCount(), 5lu);
    EXPECT_EQ(args.GetAllArguments().size(), 5lu);
    EXPECT_EQ(args[0].As<LLBC_String>(), positional);
    EXPECT_EQ(args[1].As<LLBC_String>(), mode);
    EXPECT_EQ(args[2].As<LLBC_String>(), emptyValue);
    EXPECT_EQ(args[3].As<LLBC_String>(), compoundValue);
    EXPECT_EQ(args[4].As<LLBC_String>(), noKey);

    EXPECT_EQ(args.GetNamingArgumentsCount(), 3lu);
    EXPECT_EQ(args.GetAllNamingArguments().size(), 3lu);
    EXPECT_EQ(args["mode"].As<LLBC_String>(), "debug");
    EXPECT_EQ(args["empty"].As<LLBC_String>(), "");
    EXPECT_EQ(args["filter"].As<LLBC_String>(), "a=b");

    EXPECT_TRUE(args[99].Is<void>());
    EXPECT_TRUE(args["missing"].Is<void>());
}

// Parse() is reusable. A second command line must replace rather than append to
// the previous state; this verifies the parsed flag and reset path.
TEST(MiscUtilTest, StartArgsReparseResetsPriorState)
{
    char firstModule[] = "first";
    char firstArg[] = "a=1";
    char *firstArgv[] = {firstModule, firstArg};

    LLBC_StartArgs args;
    ASSERT_EQ(args.Parse(static_cast<int>(std::size(firstArgv)), firstArgv), LLBC_OK);
    ASSERT_TRUE(args.IsParsed());

    char secondModule[] = "second";
    char secondPositional[] = "plain";
    char secondArg[] = "b=2";
    char *secondArgv[] = {secondModule, secondPositional, secondArg};
    ASSERT_EQ(args.Parse(static_cast<int>(std::size(secondArgv)), secondArgv), LLBC_OK);

    EXPECT_TRUE(args.IsParsed());
    EXPECT_EQ(args.GetModuleFilePath(), secondModule);
    EXPECT_EQ(args.GetArgumentsCount(), 2lu);
    EXPECT_EQ(args[0].As<LLBC_String>(), secondPositional);
    EXPECT_EQ(args[1].As<LLBC_String>(), secondArg);
    EXPECT_EQ(args.GetNamingArgumentsCount(), 1lu);
    EXPECT_TRUE(args["a"].Is<void>());
    EXPECT_EQ(args["b"].As<LLBC_String>(), "2");
}

// Invalid command-line metadata is rejected and does not falsely mark an object
// parsed. This protects callers that use IsParsed() as a parse-success guard.
TEST(MiscUtilTest, StartArgsRejectsInvalidInput)
{
    LLBC_StartArgs args;

    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_EQ(args.Parse(0, nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    EXPECT_FALSE(args.IsParsed());
    EXPECT_EQ(args.GetArgumentsCount(), 0lu);
    EXPECT_EQ(args.GetNamingArgumentsCount(), 0lu);
    EXPECT_TRUE(args[0].Is<void>());
    EXPECT_TRUE(args["missing"].Is<void>());
}
