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

#include "TestSuite.h"
#include "TestTraits.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(push)
#pragma warning(disable: 4996)
#endif // LLBC_TARGET_PLATFORM_WIN32

#define __PrintLineC(color, fmt, ...)                                \
    do                                                               \
    {                                                                \
        int olcClr = LLBC_GetConsoleColor(stdout);                   \
        LLBC_SetConsoleColor(stdout, color);                         \
        LLBC_NS __LLBC_FilePrint(false, stdout, fmt, ##__VA_ARGS__); \
        LLBC_SetConsoleColor(stdout, olcClr);                        \
        LLBC_NS __LLBC_FilePrint(false, stdout, "\n");               \
    } while (0);

#define __ClearInputBuf()                           \
    do                                              \
    {                                               \
        int c;                                      \
        while ((c = getchar()) != '\n' && c != EOF);\
    } while (0);

#define __DEPARATION_CHARACTER "--------------------------------------------------"

int TestSuite_Main(int argc, char* argv[])
{
    LLBC_Startup();
    LLBC_HookProcessCrash();
    __TraitsLoop<__TEST_CASE_COUNT>::Generate();

    while (true)
    {
        __PrintLineC(LLBC_NS LLBC_ConsoleColor::Bg_Green, __DEPARATION_CHARACTER);
        for (int i = 0; i < __TEST_CASE_COUNT; ++i)
        {
            const char* testcaseName = __TEST_CASE_NAME(i);
            __TestCaseFactoryFunc testcaseFactory = __TEST_CASE_FUNC(i);
            if (testcaseName == nullptr || testcaseFactory == nullptr)
                continue;

            LLBC_PrintLn("%d: %s", i + 1, testcaseName);
        }
        __PrintLineC(LLBC_NS LLBC_ConsoleColor::Bg_Green, __DEPARATION_CHARACTER);

        int idx = -1;
        LLBC_Print("Please select testcase (0-exit): ");

        char inputBuf[8192];
        if (fgets(inputBuf, sizeof(inputBuf), stdin) == nullptr)
        {
            // __ClearInputBuf();
            continue;
        }

        // __ClearInputBuf();
        const LLBC_Strings inputs = LLBC_String(inputBuf).strip().split(' ');
        idx = LLBC_Str2Int32(inputs[0].c_str());
        if (idx <= 0)
        {
            break;
        }
        else if (--idx >= __TEST_CASE_COUNT)
        {
            __PrintLineC(LLBC_NS LLBC_ConsoleColor::Fg_Red, "unimplemented test case.");
            continue;
        }

        const char* testcaseName = __TEST_CASE_NAME(idx);
        __TestCaseFactoryFunc testcaseFactory = __TEST_CASE_FUNC(idx);
        if (testcaseName == nullptr || testcaseFactory == nullptr)
        {
            __PrintLineC(LLBC_NS LLBC_ConsoleColor::Fg_Red, "unimplemented test case.");
            continue;
        }

        ::llbc::LLBC_ITestCase* test = testcaseFactory();
        if (!test)
        {
            __PrintLineC(LLBC_NS LLBC_ConsoleColor::Fg_Red, "unimplemented test case.");
            continue;
        }

        __PrintLineC(LLBC_NS LLBC_ConsoleColor::Bg_White, "%s selected.", testcaseName);

        int testArgc = static_cast<int>(inputs.size());
        const char **testArgv = LLBC_Malloc(const char *, sizeof(char *) * testArgc);
        testArgv[0] = argv[0];
        for (int i = 1; i < testArgc; ++i)
            testArgv[i] = inputs[i].c_str();

        int testCaseRet = test->Run(testArgc, const_cast<char **>(testArgv));
        free(testArgv);
        delete test;

        if (testCaseRet != LLBC_OK)
        {
            __PrintLineC(LLBC_NS LLBC_ConsoleColor::Fg_Red,
                         "%s run failed, retCode:%d, press any key to continue...",
                         testcaseName, testCaseRet);
            getchar();
        }
    }

    ::llbc::LLBC_Cleanup();
    return 0;
}

#undef __PrintLineC
#undef __DEPARATION_CHARACTER
#undef __ClearInputBuf
 
#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(pop)
#endif // LLBC_TARGET_PLATFORM_WIN32

