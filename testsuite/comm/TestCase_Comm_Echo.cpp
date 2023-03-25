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


#include "comm/TestCase_Comm_Echo.h"

namespace
{

class EchoServerComp : public LLBC_Component
{
public:
    virtual bool OnInit(bool &initFinished)
    {
        // 消息注册, 因echo实现为流式协议实现, opcode为0
        LLBC_Service *svc = GetService();
        svc->Subscribe(0, this, &EchoServerComp::_OnPkt);

        return true;
    }

    virtual bool OnStart(bool &startFinished)
    {
        // 执行端口监听
        LLBC_Service *svc = GetService();
        int sId = svc->Listen("0.0.0.0", 9527);
        if (sId == 0)
        {
            std::cerr << "Listening on 0.0.0.0:9527 failed, error:" << LLBC_FormatLastError() << std::endl;
            return false;
        }

        // 打印启动消息
        std::cout << "Echo server component started, listening on 0.0.0.0:9527, session Id:" << sId << std::endl;

        return true;
    }

    virtual void OnStop(bool &stopFinished)
    {
        // 打印将关闭消息
        std::cout << "Echo server component will stop" << std::endl;
    }

    virtual void OnSessionCreate(const LLBC_SessionInfo &sessionInfo)
    {
        if (!sessionInfo.IsListenSession())
            std::cout << "Session[sId:" << sessionInfo.GetSessionId()
                      << ", addr:" << sessionInfo.GetPeerAddr() << "]: <create>" << std::endl;
    }

    virtual void OnSessionDestroy(const LLBC_SessionDestroyInfo &destroyInfo)
    {
        if (!destroyInfo.GetSessionInfo().IsListenSession())
        {
            auto &sessionInfo = destroyInfo.GetSessionInfo();
            std::cout << "Session[sId:" << sessionInfo.GetSessionId()
                      << ", addr:" << sessionInfo.GetPeerAddr() << "]: <destroy, reason:" 
                      << destroyInfo.GetReason() << ">" << std::endl;
        }
    }

private:
    void _OnPkt(LLBC_Packet &pkt)
    {
        // Log
        LLBC_Service *svc = GetService();
        const char *msg = reinterpret_cast<const char *>(pkt.GetPayload());
        std::cout << "Session[sId:" << pkt.GetSessionId() << ", addr:" << pkt.GetPeerAddr() << "]: " << msg << std::endl;

        // 回显
        svc->Send(pkt.GetSessionId(), pkt.GetPayload(), pkt.GetPayloadLength());
    }
};

class EchoClientComp : public LLBC_Component
{
public:
    virtual bool OnInit(bool &initFinished)
    {
        // 同EchoServerComp, 订阅
        LLBC_Service *svc = GetService();
        svc->Subscribe(0, this, &EchoClientComp::_OnPkt);

        return true;
    }

    virtual bool OnStart(bool &startFinished)
    {
        LLBC_Service *svc = GetService();
        _sId = svc->Connect("127.0.0.1", 9527);
        if (_sId == 0)
        {
            std::cerr << "Connect to echo server failed, error:" << LLBC_FormatLastError() << std::endl;
            return false;
        }

        std::cout << "Echo client comp started, input echo string, input 'bye' to exit echo client" << std::endl;
        std::flush(std::cout);

        if (!_AcceptInput(true))
            return false;

        return true;
    }

    virtual void OnStop(bool &stopFinished)
    {
        std::cout << "Echo client comp will stop" << std::endl;
    }

private:
    void _OnPkt(LLBC_Packet &pkt)
    {
        // 打印回显
        const char *msg = reinterpret_cast<const char *>(pkt.GetPayload());
        std::cout << "[Echo]" << msg << std::endl;

        // 接受新输入
        _AcceptInput();
    }

    bool _AcceptInput(bool inStart = false)
    {
        // 输入
        std::string input;
        while (input.empty())
        {
            std::cin >> input;
            if (input == "bye")
            {
                std::cout << "Bye~" << std::endl;
                if (!inStart)
                    GetService()->Stop();

                return false;
            }
        }

        // 发送
        LLBC_Service *svc = GetService();
        svc->Send(_sId, input.data(), input.size() + 1);

        return true;
    }

private:
    int _sId;
};

}

TestCase_Comm_Echo::TestCase_Comm_Echo()
{
}

TestCase_Comm_Echo::~TestCase_Comm_Echo()
{
}

int TestCase_Comm_Echo::Run(int argc, char *argv[])
{
    std::cout <<"Echo test:" <<std::endl;

    // 选择启动方式
    std::cout << "Start echo as server(Y/[N])?";

    bool asServer;
    LLBC_String choice;
    std::cin >> choice;
    choice = choice.strip().toupper();
    asServer = choice == "Y" || choice == "YES";

    // 创建&启动service
    LLBC_Service *svc = LLBC_Service::Create("EchoSvc", new LLBC_RawProtocolFactory);
    if (asServer)
        svc->AddComponent(new EchoServerComp);
    else
        svc->AddComponent(new EchoClientComp);

    if (svc->Start() != LLBC_OK)
    {
        std::cerr << "Start service failed, error:" << LLBC_FormatLastError() << std::endl;
        delete svc;
        return LLBC_FAILED;
    }

    // if is client, wait service, otherwise press any key to exit test case(server).
    if (asServer)
    {
        std::cout <<"Press any key to exit..." <<std::endl;
        getchar();
        getchar();
    }
    else
    {
        svc->Wait();
    }

    // 删除service
    delete svc;

    return LLBC_OK;
}

