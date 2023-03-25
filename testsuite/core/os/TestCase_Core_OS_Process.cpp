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


#include "core/os/TestCase_Core_OS_Process.h"

int TestCase_Core_OS_Process::Run(int argc, char *argvp[])
{
    std::cout <<"core/os/process test:" <<std::endl;

    // Test crash hook.
    if (TestCrash() != LLBC_OK)
        return LLBC_FAILED;

    std::cout <<"Press any key to continue..." <<std::endl;
    getchar();

    return 0;
}

int TestCase_Core_OS_Process::TestCrash()
{
    std::cout << "Crash hook test:" << std::endl;

#if LLBC_SUPPORT_HOOK_PROCESS_CRASH
    // Set crash hook.
    std::cout << "Hook process crash..." << std::endl;
    if (LLBC_HookProcessCrash() != LLBC_OK)
    {
        std::cerr << "Hook process crash failed, err:" << LLBC_FormatLastError() << std::endl;
        return LLBC_FAILED;
    }

    TestCrash_DivisionByZero();
    TestCrash_InvalidPtrRead();
    TestCrash_InvalidPtrWrite();

    return LLBC_OK;
#else
    // Unsupported hook process crash.
    return LLBC_OK;
#endif
}

void TestCase_Core_OS_Process::TestCrash_DivisionByZero()
{
    std::cout << "Trigger division by 0 exception..." << std::endl;
    std::cout << "3 / 0 = " << 3 / LLBC_Str2Int32("0") << std::endl;
}

void TestCase_Core_OS_Process::TestCrash_InvalidPtrWrite()
{
    std::cout << "Test invalid pointer write" << std::endl;

    int *invalidPtr4Write = nullptr;
    *invalidPtr4Write = 3;

    std::cout << *invalidPtr4Write << std::endl;
}

void TestCase_Core_OS_Process::TestCrash_InvalidPtrRead()
{
    LLBC_PrintLn("Test invalid pointer read");

    int *invalidPtr4Write = nullptr;
    std::cout << *invalidPtr4Write << std::endl;
}
 