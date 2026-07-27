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

#include "comm/Example_Comm_Poller.h"

int Example_Comm_Poller::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm Poller example: Poller model and poller-count configuration");

    // A Poller is the multiplexing backend for network I/O events. llbc automatically picks the concrete implementation per platform:
    //   - Windows: IocpPoller
    //   - Linux/Android: EpollPoller
    //   - Others (including macOS): SelectPoller
    // The concrete type is decided at compile time by the platform and cannot be switched via the public Service interface;
    // what is tunable is the poller count (pollerCount), used to spread sessions across multiple I/O threads.

    // Print the poller types available on the current platform (iterate the LLBC_PollerType enum).
    LLBC_PrintLn("- Poller types available on this platform:");
    for (int type = LLBC_PollerType::Begin; type != LLBC_PollerType::End; ++type)
        LLBC_PrintLn("  * type:%d -> %s", type, LLBC_PollerType::Type2Str(type).c_str());

    // Configure the poller count via LLBC_ServiceStartArgs, then start.
    LLBC_Service *svc = LLBC_Service::Create("PollerSvc");

    LLBC_ServiceStartArgs startArgs;
    startArgs.pollerCount = 2; // Use 2 pollers (default is 1).
    LLBC_PrintLn("- Starting Service with pollerCount=%d...", startArgs.pollerCount);

    if (svc->Start(startArgs) != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start Service, error:%s", LLBC_FormatLastError());
        delete svc;
        return LLBC_OK;
    }

    LLBC_PrintLn("- Service started (IsStarted:%s)", svc->IsStarted() ? "true" : "false");

    // Idle for a moment (bounded), then stop.
    LLBC_Sleep(200);
    svc->Stop();
    delete svc;
    LLBC_PrintLn("- Service destroyed");

    return LLBC_OK;
}
