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


#include "comm/TestCase_Comm_ProtoStackCtrl.h"

namespace
{
    class TestPackProtocol : public LLBC_PacketProtocol
    {
    public:
        virtual bool Ctrl(int ctrlType, const LLBC_Variant &ctrlData)
        {
            LLBC_PrintLine(">>>Ctrl protocol, type:%d, data:%s", ctrlType, ctrlData.ToString().c_str());
            return true;
        }
    };

    class TestProtoFactory : public LLBC_IProtocolFactory
    {
    public:
        virtual LLBC_IProtocol *Create(int layer) const
        {
            switch (layer)
            {
                case LLBC_ProtocolLayer::PackLayer:
                    return new TestPackProtocol();

                case LLBC_ProtocolLayer::CodecLayer:
                    return new LLBC_CodecProtocol();

                case LLBC_ProtocolLayer::CompressLayer:
                    return new LLBC_CompressProtocol();

                default:
                    return NULL;
            }
        }
    };
}

TestCase_Comm_ProtoStackCtrl::TestCase_Comm_ProtoStackCtrl()
: _svc(LLBC_IService::Create(LLBC_IService::Normal, "ProtoStackCtrl"))
{
}

TestCase_Comm_ProtoStackCtrl::~TestCase_Comm_ProtoStackCtrl()
{
    LLBC_Delete(_svc);
}

int TestCase_Comm_ProtoStackCtrl::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Communication Service protocol stack control test:");
    LLBC_PrintLine("Note: Maybe you must use gdb or windbg to trace!");

    // Start service.
    LLBC_PrintLine("Start service...");
    int ret = _svc->Start();
    if (ret != LLBC_OK)
    {
        LLBC_FilePrintLine(stderr, "Start service failed, error:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    // Add listen session.
    LLBC_PrintLine("Listen on 127.0.0.1:10086...");
    int sessionId = _svc->Listen("127.0.0.1", 10086, new TestProtoFactory());
    if (sessionId == 0)
    {
        LLBC_FilePrintLine(stderr, "Listen on 127.0.0.1:10086 failed, error:%s", LLBC_FormatLastError());
        return LLBC_FAILED;
    }

    // Ctrl service
    int ctrlType;
    LLBC_Variant ctrlData;
    ctrlType = 10086;
    ctrlData["Hello"] = "World";
    TestCtrlScene(sessionId, ctrlType, ctrlData);

    ctrlType = 10010;
    ctrlData = 0x7fffffff;
    TestCtrlScene(sessionId, ctrlType, ctrlData);

    ctrlType = 10010;
    ctrlData = true;
    TestCtrlScene(sessionId, ctrlType, ctrlData);

    LLBC_FlushFile(stdout);
    LLBC_FlushFile(stderr);

    #if LLBC_TARGET_PLATFORM_IPHONE
    LLBC_PrintLine("Sleep 300 seconds to exit...");
    LLBC_ThreadManager::Sleep(300 * 1000);
    #else // Non-iPhone
    LLBC_PrintLine("Press any key to continue...");
    getchar();
    #endif // iPhone

    return 0;
}

void TestCase_Comm_ProtoStackCtrl::TestCtrlScene(int sessionId, int ctrlType, const LLBC_Variant &ctrlData)
{
    LLBC_PrintLine("Do control scene, type:%d, data:%s", ctrlType, ctrlData.ToString().c_str());
    _svc->CtrlProtocolStack(sessionId, ctrlType, ctrlData);
}
