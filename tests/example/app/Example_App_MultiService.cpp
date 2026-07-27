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

#include "app/Example_App_MultiService.h"

namespace
{

// A minimal business component: only prints in lifecycle hooks, used to observe
// the Service starting/stopping together with the App.
class DemoComp final : public LLBC_Component
{
public:
    int OnStart(bool &startFinished) override
    {
        LLBC_PrintLn("    -> DemoComp::OnStart");
        return LLBC_OK;
    }

    void OnStop(bool &stopFinished) override
    {
        LLBC_PrintLn("    <- DemoComp::OnStop");
    }
};

// A demo App: "composes" (creates and starts) multiple Services in the OnStart hook.
class DemoApp final : public LLBC_App
{
public:
    int OnStart(int argc, char *argv[], bool &startFinished) override
    {
        LLBC_PrintLn("  App::OnStart, name:%s -- composing multiple Services", GetName().c_str());

        // Multi-Service composition: a single App hosts multiple Services at once.
        for (const char *svcName : {"GameSvc", "LogSvc"})
        {
            LLBC_Service *svc = LLBC_Service::Create(svcName);
            svc->AddComponent(new DemoComp);
            if (svc->Start() != LLBC_OK)
            {
                LLBC_PrintLn("  Start service '%s' failed, err:%s", svcName, LLBC_FormatLastError());
                return LLBC_FAILED;
            }
            LLBC_PrintLn("  - Service '%s' started", svcName);
        }

        return LLBC_OK;
    }

    void OnStop(bool &stopFinished) override
    {
        // The App will uniformly stop/destroy all Services under it; no manual delete needed here.
        LLBC_PrintLn("  App::OnStop -- App stops and destroys all owned Services");
    }
};

void StopApp(void *arg)
{
    LLBC_Sleep(500);
    static_cast<DemoApp *>(arg)->Stop();
}

} // anonymous namespace

int Example_App_MultiService::Run(int argc, char *argv[])
{
    // Note: LLBC_App::Start() also attempts LLBC_Startup() internally; once it detects
    //       re-entry (REENTRY) it skips automatically and records that "the framework was
    //       not initialized by the App", so the App will not clean up the framework on exit
    //       -- allowing the menu driver to keep running.

    LLBC_PrintLn("LLBC_App multi-Service composition example:");
    LLBC_PrintLn("- App::Start() blocks until it is stopped (by a signal or Stop()).");
    LLBC_PrintLn("- An LLBC background thread calls app.Stop() after about 500 ms for graceful shutdown.");

    // Create the App object (only one App instance is allowed in the process at a time).
    DemoApp app;

    // LLBC background thread: later calls Stop() from a "non-startup thread".
    // When called from a non-startup thread, Stop() only posts a stop event and returns
    // immediately; the blocking Start() on the main thread handles it and exits gracefully.
    LLBC_NativeThreadHandle stopper = LLBC_INVALID_NATIVE_THREAD_HANDLE;
    if (LLBC_CreateThread(&StopApp, &app, &stopper) != LLBC_OK)
    {
        LLBC_PrintLn("Failed to create stop thread, error:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    // Start the App (runs blocking until the Stop() above takes effect).
    const int ret = app.Start(argc, argv, "ExampleApp");

    // Wait for the LLBC background thread to finish.
    const int joinRet = LLBC_JoinThread(stopper);
    if (joinRet != LLBC_OK)
        LLBC_PrintLn("Failed to join stop thread, error:%s", LLBC_FormatLastError());

    if (ret != LLBC_OK)
    {
        LLBC_PrintLn("App start failed, ret:%d, err:%s", ret, LLBC_FormatLastError());
        return ret;
    }

    if (joinRet != LLBC_OK)
        return joinRet;

    LLBC_PrintLn("App stopped gracefully; example complete.");
    return LLBC_OK;
}
