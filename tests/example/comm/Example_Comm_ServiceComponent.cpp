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

#include "comm/Example_Comm_ServiceComponent.h"

namespace
{

// A minimal business component: business logic is attached to the Service as an LLBC_Component subclass.
class GreeterComp final : public LLBC_Component
{
public:
    // OnInit: component init hook (fires during the Service startup init phase, before OnStart).
    int OnInit(bool &initFinished) override
    {
        LLBC_PrintLn("  [GreeterComp] OnInit: component initialized");
        return LLBC_OK;
    }

    // OnStart: component start hook (fires after all components' OnInit complete).
    int OnStart(bool &startFinished) override
    {
        LLBC_PrintLn("  [GreeterComp] OnStart: component started, service:%s", GetService()->GetName().c_str());
        return LLBC_OK;
    }

    // OnStop: component stop hook.
    void OnStop(bool &stopFinished) override
    {
        LLBC_PrintLn("  [GreeterComp] OnStop: component stopped");
    }

public:
    void SayHello()
    {
        LLBC_PrintLn("  [GreeterComp] Hello from component method!");
    }
};

}

int Example_Comm_ServiceComponent::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm ServiceComponent example: define a component and attach it to a Service");

    // 1) Create the Service.
    LLBC_Service *svc = LLBC_Service::Create("CompSvc");

    // 2) Attach the component. AddComponent(LLBC_Component *) takes over ownership of the component.
    svc->AddComponent(new GreeterComp);
    LLBC_PrintLn("- GreeterComp attached; starting...");

    // 3) Start the Service; each component's OnInit -> OnStart fires in turn.
    if (svc->Start() != LLBC_OK)
    {
        LLBC_PrintLn("- Failed to start Service, error:%s", LLBC_FormatLastError());
        delete svc;
        return LLBC_OK;
    }

    // 4) After startup, the component can be fetched back by type and its methods called.
    GreeterComp *comp = svc->GetComponent<GreeterComp>();
    if (comp != nullptr)
    {
        LLBC_PrintLn("- GetComponent<GreeterComp>() succeeded: %p", comp);
        comp->SayHello();
    }

    // 5) Idle for a moment (bounded), then stop, triggering OnStop.
    LLBC_Sleep(200);
    svc->Stop();

    delete svc;
    LLBC_PrintLn("- Service destroyed");

    return LLBC_OK;
}
