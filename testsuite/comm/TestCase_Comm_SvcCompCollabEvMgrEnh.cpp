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


#include "comm/TestCase_Comm_SvcCompCollabEvMgrEnh.h"

namespace
{
    LLBC_EventMgr *eventMgr;
    enum EventIds
    {
        TEST_EV_INIT = 1,
        TEST_EV_LATE_INIT = 2,
        TEST_EV_START = 3,
        TEST_EV_LATE_START = 4,
        TEST_EV_RUNNING = 5,
    };

    class TestTemplateComp : public LLBC_Component
    {
    public:
        TestTemplateComp(LLBC_String name) : comp_name(name) {};
    public:
        int OnInit(bool &finished) override
        {
            test_stub_init = eventMgr->AddListener(TEST_EV_INIT, this, &TestTemplateComp::OnEv_TestInitHandle);
            LLBC_PrintLn("%s init!", comp_name.c_str());
            return LLBC_OK;
        }
        
        int OnLateInit(bool &finished) override
        {
            test_stub_late_init = eventMgr->AddListener(TEST_EV_LATE_INIT, this, &TestTemplateComp::OnEv_TestLateInitHandle);
            LLBC_PrintLn("%s late init!", comp_name.c_str());
            return LLBC_OK;
        }
        
        void OnEarlyDestroy(bool &finished) override
        {
            LLBC_PrintLn("\n--%s early destroy!", comp_name.c_str());

            int old_exec_late_init = exec_late_init;
            LLBC_PrintLn("    %s late init event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_LATE_INIT).Fire();
            ASSERT(old_exec_late_init == exec_late_init);

            int old_exec_init = exec_init;
            LLBC_PrintLn("    %s init event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_INIT).Fire();
            ASSERT(old_exec_init + (test_stub_init ? 1 : 0) == exec_init);
        }
        
        void OnDestroy(bool &finished) override
        {
            LLBC_PrintLn("\n--%s destroy!", comp_name.c_str());

            int old_exec_init = exec_init;
            LLBC_PrintLn("    %s init event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_INIT).Fire();
            ASSERT(old_exec_init == exec_init);
        }
        
        int OnStart(bool &finished) override
        {
            test_stub_start = eventMgr->AddListener(TEST_EV_START, this, &TestTemplateComp::OnEv_TestStartHandle);
            LLBC_PrintLn("%s start!", comp_name.c_str());
            return LLBC_OK;
        }
        
        int OnLateStart(bool &finished) override
        {
            test_stub_late_start = eventMgr->AddListener(TEST_EV_LATE_START, this, &TestTemplateComp::OnEv_TestLateStartHandle);
            LLBC_PrintLn("%s late start!", comp_name.c_str());
            return LLBC_OK;
        }
        
        void OnEarlyStop(bool &finished) override
        {
            LLBC_PrintLn("\n--%s early stop!", comp_name.c_str());

            int old_exec_running = exec_running;
            LLBC_PrintLn("    %s running event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_RUNNING).Fire();
            ASSERT(old_exec_running == exec_running);

            int old_exec_late_start = exec_late_start;
            LLBC_PrintLn("    %s late start event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_LATE_START).Fire();
            ASSERT(old_exec_late_start == exec_late_start);

            int old_exec_start = exec_start;
            LLBC_PrintLn("    %s start event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_START).Fire();
            ASSERT(old_exec_start + (test_stub_start ? 1 : 0) == exec_start);

            int old_exec_late_init = exec_late_init;
            LLBC_PrintLn("    %s late init event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_LATE_INIT).Fire();
            ASSERT(old_exec_late_init + (test_stub_late_init ? 1 : 0) == exec_late_init);

            int old_exec_init = exec_init;
            LLBC_PrintLn("    %s init event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_INIT).Fire();
            ASSERT(old_exec_init + (test_stub_init ? 1 : 0) == exec_init);
        }
        
        void OnStop(bool &finished) override
        {
            LLBC_PrintLn("\n--%s stop!", comp_name.c_str());

            int old_exec_start = exec_start;
            LLBC_PrintLn("    %s start event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_START).Fire();
            ASSERT(old_exec_start == exec_start);

            int old_exec_late_init = exec_late_init;
            LLBC_PrintLn("    %s late init event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_LATE_INIT).Fire();
            ASSERT(old_exec_late_init + (test_stub_late_init ? 1 : 0) == exec_late_init);

            int old_exec_init = exec_init;
            LLBC_PrintLn("    %s init event fire!", comp_name.c_str());
            eventMgr->BeginFire(TEST_EV_INIT).Fire();
            ASSERT(old_exec_init + (test_stub_init ? 1 : 0) == exec_init);
        }

        void OnRunning()
        {
            test_stub_running = eventMgr->AddListener(TEST_EV_RUNNING, this, &TestTemplateComp::OnEv_TestRunningHandle);
            LLBC_PrintLn("%s running!", comp_name.c_str());
        }

        void RemoveRunningListenStub()
        {
            eventMgr->RemoveListenerX(test_stub_running);
        }
        
    private:
        void OnEv_TestInitHandle(LLBC_Event &ev)
        {
            LLBC_PrintLn("        %s TestInitHandle!", comp_name.c_str());
            exec_init += 1;
        }

        void OnEv_TestLateInitHandle(LLBC_Event &ev)
        {
            LLBC_PrintLn("        %s TestLateInitHandle!", comp_name.c_str());
            exec_late_init += 1;
        }

        void OnEv_TestStartHandle(LLBC_Event &ev)
        {
            LLBC_PrintLn("        %s TestStartHandle!", comp_name.c_str());
            exec_start += 1;
        }

        void OnEv_TestLateStartHandle(LLBC_Event &ev)
        {
            LLBC_PrintLn("        %s TestLateStartHandle!", comp_name.c_str());
            exec_late_start += 1;
        }

        void OnEv_TestRunningHandle(LLBC_Event &ev)
        {
            LLBC_PrintLn("        %s TestRunningHandle!", comp_name.c_str());
            exec_running += 1;
        }

    public:
        LLBC_String comp_name;

        LLBC_ListenerStub test_stub_init = 0;
        int exec_init = 0;
        LLBC_ListenerStub test_stub_late_init = 0;
        int exec_late_init = 0;
        LLBC_ListenerStub test_stub_start = 0;
        int exec_start = 0;
        LLBC_ListenerStub test_stub_late_start = 0;
        int exec_late_start = 0;
        LLBC_ListenerStub test_stub_running = 0;
        int exec_running = 0;
    };

    class TestCompA : public TestTemplateComp
    {
    public:
        TestCompA() : TestTemplateComp("TestCompA") {}
    };

    class TestCompB : public TestTemplateComp
    {
    public:
        TestCompB() : TestTemplateComp("TestCompB") {}
    };
}

void TestCase_Comm_SvcCompCollabEvMgrEnh::NormalTest()
{
    LLBC_PrintLn("\n\n--------begin normal test!--------");

    // Create service
    LLBC_Service *svc = LLBC_Service::Create("SvcTest");

    eventMgr = new LLBC_EventMgr;
    svc->AddCollaborativeEventMgr(eventMgr);

    svc->AddComponent<TestCompA>();
    svc->AddComponent<TestCompB>();

    svc->Start();

    LLBC_PrintLn("Service running!");
    auto * comp_a = svc->GetComponent<TestCompA>();
    auto * comp_b = svc->GetComponent<TestCompB>();
    comp_a->OnRunning();

    int old_exec_running_a = comp_a->exec_running;
    int old_exec_running_b = comp_b->exec_running;
    LLBC_PrintLn("\n    running event fire!");
    eventMgr->BeginFire(TEST_EV_RUNNING).Fire();
    ASSERT(old_exec_running_a + 1 == comp_a->exec_running);
    ASSERT(old_exec_running_b == comp_b->exec_running);

    int old_exec_late_start_a = comp_a->exec_late_start;
    int old_exec_late_start_b = comp_b->exec_late_start;
    LLBC_PrintLn("\n    late start event fire!");
    eventMgr->BeginFire(TEST_EV_LATE_START).Fire();
    ASSERT(old_exec_late_start_a + 1 == comp_a->exec_late_start);
    ASSERT(old_exec_late_start_b + 1 == comp_b->exec_late_start);

    int old_exec_start_a = comp_a->exec_start;
    int old_exec_start_b = comp_b->exec_start;
    LLBC_PrintLn("\n    start event fire!");
    eventMgr->BeginFire(TEST_EV_START).Fire();
    ASSERT(old_exec_start_a + 1 == comp_a->exec_start);
    ASSERT(old_exec_start_b + 1 == comp_b->exec_start);

    int old_exec_late_init_a = comp_a->exec_late_init;
    int old_exec_late_init_b = comp_b->exec_late_init;
    LLBC_PrintLn("\n    late init event fire!");
    eventMgr->BeginFire(TEST_EV_LATE_INIT).Fire();
    ASSERT(old_exec_late_init_a + 1 == comp_a->exec_late_init);
    ASSERT(old_exec_late_init_b + 1 == comp_b->exec_late_init);

    int old_exec_init_a = comp_a->exec_init;
    int old_exec_init_b = comp_b->exec_init;
    LLBC_PrintLn("\n    init event fire!");
    eventMgr->BeginFire(TEST_EV_INIT).Fire();
    ASSERT(old_exec_init_a + 1 == comp_a->exec_init);
    ASSERT(old_exec_init_b + 1 == comp_b->exec_init);

    svc->Stop();

    delete svc;
    delete eventMgr;
    LLBC_PrintLn("---------end normal test!--------");
}

void TestCase_Comm_SvcCompCollabEvMgrEnh::EarlyRemoveTest()
{
    LLBC_PrintLn("\n\n--------begin early remove test!--------");

    // Create service
    LLBC_Service *svc = LLBC_Service::Create("SvcTest");

    eventMgr = new LLBC_EventMgr;
    svc->AddCollaborativeEventMgr(eventMgr);

    svc->AddComponent<TestCompA>();
    svc->AddComponent<TestCompB>();

    svc->Start();

    LLBC_PrintLn("Service running!");
    auto * comp_a = svc->GetComponent<TestCompA>();
    auto * comp_b = svc->GetComponent<TestCompB>();
    comp_a->OnRunning();

    int old_exec_running_a = comp_a->exec_running;
    int old_exec_running_b = comp_b->exec_running;
    LLBC_PrintLn("    running event fire!");
    eventMgr->BeginFire(TEST_EV_RUNNING).Fire();
    ASSERT(old_exec_running_a + 1 == comp_a->exec_running);
    ASSERT(old_exec_running_b == comp_b->exec_running);

    comp_a->RemoveRunningListenStub();
    LLBC_PrintLn("compA remove running stub!");

    old_exec_running_a = comp_a->exec_running;
    old_exec_running_b = comp_b->exec_running;
    LLBC_PrintLn("    running event fire!");
    eventMgr->BeginFire(TEST_EV_RUNNING).Fire();
    ASSERT(old_exec_running_a == comp_a->exec_running);
    ASSERT(old_exec_running_b == comp_b->exec_running);

    eventMgr->RemoveListenerX(comp_a->test_stub_late_start);
    eventMgr->RemoveListenerX(comp_b->test_stub_start);
    eventMgr->RemoveListenerX(comp_a->test_stub_late_init);
    eventMgr->RemoveListenerX(comp_b->test_stub_init);

    int old_exec_late_start_a = comp_a->exec_late_start;
    int old_exec_late_start_b = comp_b->exec_late_start;
    LLBC_PrintLn("    late start event fire!");
    eventMgr->BeginFire(TEST_EV_LATE_START).Fire();
    ASSERT(old_exec_late_start_a == comp_a->exec_late_start);
    ASSERT(old_exec_late_start_b + 1 == comp_b->exec_late_start);

    int old_exec_start_a = comp_a->exec_start;
    int old_exec_start_b = comp_b->exec_start;
    LLBC_PrintLn("    start event fire!");
    eventMgr->BeginFire(TEST_EV_START).Fire();
    ASSERT(old_exec_start_a + 1 == comp_a->exec_start);
    ASSERT(old_exec_start_b == comp_b->exec_start);

    int old_exec_late_init_a = comp_a->exec_late_init;
    int old_exec_late_init_b = comp_b->exec_late_init;
    LLBC_PrintLn("    late init event fire!");
    eventMgr->BeginFire(TEST_EV_LATE_INIT).Fire();
    ASSERT(old_exec_late_init_a == comp_a->exec_late_init);
    ASSERT(old_exec_late_init_b + 1 == comp_b->exec_late_init);

    int old_exec_init_a = comp_a->exec_init;
    int old_exec_init_b = comp_b->exec_init;
    LLBC_PrintLn("    init event fire!");
    eventMgr->BeginFire(TEST_EV_INIT).Fire();
    ASSERT(old_exec_init_a + 1 == comp_a->exec_init);
    ASSERT(old_exec_init_b == comp_b->exec_init);

    svc->Stop();

    delete svc;
    delete eventMgr;

    LLBC_PrintLn("--------end early remove test!--------");
}

int TestCase_Comm_SvcCompCollabEvMgrEnh::Run(int argc, char *argv[])
{

    NormalTest();
    EarlyRemoveTest();

    std::cout <<"press any key to continue ..." <<std::endl;
    getchar();

    return 0;
}
