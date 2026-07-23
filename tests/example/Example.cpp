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

// All example testcases (grouped by module, order aligned with ExampleDef.h).
// - common
#include "common/Example_Common_HelloWorld.h"
#include "common/Example_Common_String.h"
#include "common/Example_Common_ErrorHandling.h"
// - core
#include "core/Example_Core_Stream.h"
#include "core/Example_Core_Variant.h"
#include "core/Example_Core_Delegate.h"
#include "core/Example_Core_Object.h"
#include "core/Example_Core_ObjPool.h"
#include "core/Example_Core_ObjbaseCollections.h"
#include "core/Example_Core_Logger.h"
#include "core/Example_Core_Timer.h"
#include "core/Example_Core_Time.h"
#include "core/Example_Core_Thread.h"
#include "core/Example_Core_Event.h"
#include "core/Example_Core_Config.h"
#include "core/Example_Core_FileDirectory.h"
#include "core/Example_Core_Random.h"
#include "core/Example_Core_Library.h"
#include "core/Example_Core_Transcoder.h"
#include "core/Example_Core_Utils.h"
// - comm
#include "comm/Example_Comm_FirstService.h"
#include "comm/Example_Comm_EchoService.h"
#include "comm/Example_Comm_ServiceComponent.h"
#include "comm/Example_Comm_LifecycleEvent.h"
#include "comm/Example_Comm_ServiceMessaging.h"
#include "comm/Example_Comm_PacketCoder.h"
#include "comm/Example_Comm_Session.h"
#include "comm/Example_Comm_Protocol.h"
#include "comm/Example_Comm_Poller.h"
// - app
#include "app/Example_App_MultiService.h"

#include "ExampleTraits.h"

int main(int argc, char *argv[])
{
    // Startup llbc library & defer cleanup llbc library.
    // Note: the framework is started up ONCE here by the menu driver; every
    //       example's Run() assumes llbc is already up and must NOT call
    //       LLBC_Startup()/LLBC_Cleanup() itself (LLBC_Startup is not re-entrant).
    LLBC_Startup();
    LLBC_Defer(LLBC_Cleanup());

    // Generate examples.
    __ExampleTraitsLoop<__EXAMPLES_COUNT>::Generate();

    // Enter examples execution loop.
    while (true)
    {
        // Flush stdout/stderr.
        LLBC_FlushFile(stdout);
        LLBC_FlushFile(stderr);

        // Print examples.
        __ExamplePrint(LLBC_ConsoleColor::Fg_Green, "\n----------------------------------------\n");
        for (int i = 0; i < __EXAMPLES_COUNT; ++i)
        {
            auto exampleName = __EXAMPLE_NAME(i);
            __ExampleFactoryFunc exampleFactoryFunc = __EXAMPLE_FUNC(i);
            LLBC_ContinueIf(!exampleName || !exampleFactoryFunc);

            LLBC_PrintLn("%d: %s", i + 1, exampleName);
        }
        __ExamplePrint(LLBC_ConsoleColor::Fg_Green, "----------------------------------------\n");

        // Waiting for user input example index.
        LLBC_Print("Please select example and arguments(if exist, 0 means exit): ");
        std::string inputLine;
        if (!std::getline(std::cin, inputLine))
            break;

        LLBC_String input(inputLine.c_str(), inputLine.size());
        const auto inputs = input.strip().split(' ', -1, true);
        LLBC_ContinueIf(inputs.empty());

        // Parse example index.
        const int index = LLBC_Str2Num<int>(inputs[0].c_str()) - 1;
        LLBC_BreakIf(index < 0);

        if (index >= __EXAMPLES_COUNT)
        {
            __ExamplePrint(LLBC_ConsoleColor::Fg_Yellow, "Unimplemented example.\n");
            continue;
        }

        auto exampleName = __EXAMPLE_NAME(index);
        __ExamplePrint(LLBC_ConsoleColor::Bg_Green, "%s", exampleName);
        LLBC_PrintLn(" selected");

        // Create example and execute.
        auto exampleArgs = LLBC_Malloc(char *, sizeof(char *) * inputs.size());
        LLBC_Defer(free(exampleArgs));
        exampleArgs[0] = argv[0];
        for (size_t i = 1; i < inputs.size(); ++i)
            exampleArgs[i] = const_cast<char *>(inputs[i].c_str());

        auto example = __EXAMPLE_FUNC(index)();
        LLBC_Defer(delete example);
        LLBC_SetLastError(LLBC_ERROR_SUCCESS);
        const auto exampleRet = example->Run(static_cast<int>(inputs.size()), exampleArgs);
        if (exampleRet != LLBC_OK)
        {
            __ExamplePrint(LLBC_ConsoleColor::Fg_Red,
                           "%s run failed, ret:%d, err:%s, press any key to continue...",
                           exampleName, exampleRet, LLBC_FormatLastError());
            getchar();
        }
    }

    return 0;
}
