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

#include "llbc.h"

#include "hello_world/QuickStart_HelloWorld.h"
#include "stream/QuickStart_Stream.h"

#include "QuickStartTraits.h"

int main(int argc, char *argv[])
{
    // Startup llbc library & defer cleanup llbc library.
    LLBC_Startup();
    LLBC_Defer(LLBC_Cleanup());

    // Generate quick starts.
    __QuickStartTraitsLoop<__QUICK_STARTS_COUNT>::Generate();

    // Enter quick starts execution loop.
    while (true)
    {
        // Flush stdout/stderr.
        LLBC_FlushFile(stdout);
        LLBC_FlushFile(stderr);

        // Print quick starts.
        __QuickStartPrint(LLBC_ConsoleColor::Fg_Green, "\n----------------------------------------\n");
        for (int i = 0; i < __QUICK_STARTS_COUNT; ++i)
        {
            auto quickStartName = __QUICK_START_NAME(i);
            __QuickStartFactoryFunc quickStartFactoryFunc = __QUICK_START_FUNC(i);
            LLBC_ContinueIf(!quickStartName || !quickStartFactoryFunc);

            LLBC_PrintLn("%d: %s", i + 1, quickStartName);
        }
        __QuickStartPrint(LLBC_ConsoleColor::Fg_Green, "----------------------------------------\n");

        // Waiting for user input quick start index.
        LLBC_Print("Please select quick start and arguments(if exist, 0 means exit): ");
        LLBC_String input;
        std::cin >> input;
        const auto inputs = input.strip().split(' ');
        LLBC_ContinueIf(inputs[0].empty());

        // Parse quick start index.
        const int index = LLBC_Str2Num<int>(inputs[0].c_str()) - 1;
        LLBC_BreakIf(index < 0);

        if (index >= __QUICK_STARTS_COUNT)
        {
            __QuickStartPrint(LLBC_ConsoleColor::Fg_Yellow, "Unplemented quick start.\n");
            continue;
        }

        auto quickStartName = __QUICK_START_NAME(index);
        __QuickStartPrint(LLBC_ConsoleColor::Bg_Green, "%s", quickStartName);
        LLBC_PrintLn(" selected");

        // Create quick start and execute.
        auto quickStartArgs = LLBC_Malloc(char *, sizeof(char *) * inputs.size());
        LLBC_Defer(free(quickStartArgs));
        quickStartArgs[0] = argv[0];
        for (size_t i = 1; i < inputs.size(); ++i)
            quickStartArgs[i] = const_cast<char *>(inputs[i].c_str());

        auto quickStart = __QUICK_START_FUNC(index)();
        LLBC_Defer(delete quickStart);
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        const auto quickStartRet = quickStart->Run(static_cast<int>(inputs.size()), quickStartArgs);
        if (quickStartRet != LLBC_OK)
        {
            __QuickStartPrint(LLBC_ConsoleColor::Fg_Red,
                              "%s run failed, ret:%d, err:%s, press any key to continue...",
                              quickStartName, quickStartRet, LLBC_FormatLastError());
            getchar();
        }
    }

    return 0;
}

