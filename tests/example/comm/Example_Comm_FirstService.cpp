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

#include "comm/Example_Comm_FirstService.h"

int Example_Comm_FirstService::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm FirstService example: minimal Service lifecycle demonstration");

    // 1) Create a Service.
    //    LLBC_Service::Create(name, dftProtocolFactory=nullptr, fullStack=true).
    //    When the protocol factory is nullptr, the library's default NormalProtocolFactory (opcode-based packet splitting) is used.
    LLBC_Service *svc = LLBC_Service::Create("FirstSvc");
    LLBC_PrintLn("- Service created, name:%s", svc->GetName().c_str());

    // 2) Start the Service. In the default SelfDrive mode, an internal thread is spawned to drive the OnSvc() frame loop.
    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start Service, error:%s", LLBC_FormatLastError());
        delete svc;
        return LLBC_OK;
    }

    // 3) Observe the running state.
    LLBC_PrintLn("- Service started: IsStarted=%s, id=%d, fps=%d",
                 svc->IsStarted() ? "true" : "false", svc->GetId(), svc->GetFPS());

    // 4) Let the Service idle for a moment (bounded wait, never blocks forever).
    LLBC_Sleep(200);

    // 5) Stop and destroy the Service. delete auto-Stops beforehand; Stop is called explicitly here to observe the flow.
    svc->Stop();
    LLBC_PrintLn("- Service stopped: IsStarted=%s", svc->IsStarted() ? "true" : "false");

    delete svc;
    LLBC_PrintLn("- Service destroyed");

    return LLBC_OK;
}
