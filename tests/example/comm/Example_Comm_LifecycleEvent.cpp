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

#include "comm/Example_Comm_LifecycleEvent.h"

namespace
{

// Demonstrates the component lifecycle hooks standardized in v1.1.1.
// Flow hooks come in pairs: Init/Destroy, Start/Stop, each with Early/Late variants:
//   Startup direction: OnInit -> OnLateInit -> OnStart -> OnLateStart
//   Shutdown direction: OnEarlyStop -> OnStop -> OnEarlyDestroy -> OnDestroy
// Non-flow events (session create/destroy, App start/stop, etc.) all go through OnEvent();
// config hot-reload goes through OnReload() (the old OnAppConfigReloaded has been removed).
// Note: each init/start/stop/destroy hook carries a bool &finished parameter;
//       setting it false makes the framework call back again next frame (for async wrap-up spread across frames), default true.
class LifecycleComp final : public LLBC_Component
{
public:
    int OnInit(bool &finished) override
    {
        LLBC_PrintLn("  1) OnInit        (initialization)");
        return LLBC_OK;
    }

    int OnLateInit(bool &finished) override
    {
        LLBC_PrintLn("  2) OnLateInit    (late initialization)");
        return LLBC_OK;
    }

    int OnStart(bool &finished) override
    {
        LLBC_PrintLn("  3) OnStart       (startup)");
        return LLBC_OK;
    }

    int OnLateStart(bool &finished) override
    {
        LLBC_PrintLn("  4) OnLateStart   (late startup)");
        return LLBC_OK;
    }

    void OnEarlyStop(bool &finished) override
    {
        LLBC_PrintLn("  5) OnEarlyStop   (early shutdown)");
    }

    void OnStop(bool &finished) override
    {
        LLBC_PrintLn("  6) OnStop        (shutdown)");
    }

    void OnEarlyDestroy(bool &finished) override
    {
        LLBC_PrintLn("  7) OnEarlyDestroy(early destruction)");
    }

    void OnDestroy(bool &finished) override
    {
        LLBC_PrintLn("  8) OnDestroy     (destruction)");
    }

    // Fires on config hot-reload.
    void OnReload() override
    {
        LLBC_PrintLn("  *) OnReload      (configuration hot reload)");
    }

    // Unified entry for non-flow events: see LLBC_ComponentEventType for eventType.
    void OnEvent(int eventType, const LLBC_Variant &eventParams) override
    {
        LLBC_PrintLn("  *) OnEvent       (non-lifecycle event, eventType:%d)", eventType);
    }
};

}

int Example_Comm_LifecycleEvent::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm LifecycleEvent example: component lifecycle hook order");

    LLBC_Service *svc = LLBC_Service::Create("LifecycleSvc");
    svc->AddComponent(new LifecycleComp);

    // Start() triggers the startup-direction hooks: OnInit -> OnLateInit -> OnStart -> OnLateStart.
    LLBC_PrintLn("- Calling Start() (triggers startup hooks):");
    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start Service, error:%s", LLBC_FormatLastError());
        delete svc;
        return LLBC_OK;
    }

    LLBC_Sleep(200);

    // Stop(true): stops and destroys the component, triggering the shutdown-direction hooks:
    //   OnEarlyStop -> OnStop -> OnEarlyDestroy -> OnDestroy.
    LLBC_PrintLn("- Calling Stop(true) (triggers shutdown hooks):");
    svc->Stop(true);

    delete svc;
    LLBC_PrintLn("- Service destroyed");

    return LLBC_OK;
}
