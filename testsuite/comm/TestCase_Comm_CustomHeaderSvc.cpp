/**
 * @file    TestCase_Comm_CustomHeaderSvc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/10/01
 * @version 1.0
 *
 * @brief
 */

#include "comm/TestCase_Comm_CustomHeaderSvc.h"

namespace
{
    int OPCODE = 10086;

    struct Msg
    {
        sint8 i8Val;
        sint32 i32Val;
        float floatVal;
        double doubleVal;
    };

    class Facade : public LLBC_IFacade
    {
    public:
        Facade(bool asClient)
        {
            _asClient = asClient;
            _lastUpdateTime = 0;
        }

    public:
        virtual void OnUpdate()
        {
            if (_asClient)
                return;

            time_t now = time(NULL);
            if (now - _lastUpdateTime < 1)
                return;

            Msg msg;
            msg.i8Val = 'a';
            msg.i32Val = static_cast<sint32>(now);
            msg.floatVal = 1.618f;
            msg.doubleVal = 8.161;

            // Set header parts value.
            LLBC_PacketHeaderParts *parts = LLBC_New(LLBC_PacketHeaderParts);
            parts->SetPart(2, (const char *)("llbc"));
            parts->SetPart(3, 64);
            parts->SetPart(4, 888888888888L);
            parts->SetPart(5, 3.14159);
            parts->SetPart(6, 3.14159f);

            GetService()->Broadcast2(OPCODE, msg, 0, parts);

            _lastUpdateTime = now;
        }

    public:
        void OnRecv(LLBC_Packet &packet)
        {
            Msg msg;
            packet >>msg;
            LLBC_PrintLine("Recv msg, msg.i8Val: %c, "
                "msg.i32Val: %d, msg.floatVal: %f, msg.doubleVal: %f",
                msg.i8Val, msg.i32Val, msg.floatVal, msg.doubleVal);
            LLBC_PrintLine("  Msg length: %d", packet.GetLength());
            LLBC_PrintLine("  Msg opcode: %d", packet.GetOpcode());
            LLBC_PrintLine("  Msg status: %d", packet.GetStatus());
            LLBC_PrintLine("  Part 2 value: %s", packet.GetHeaderPartAsStr(2).c_str());
            LLBC_PrintLine("  Part 3 value(as sint32): %d", packet.GetHeaderPartAsSInt32(3));
            LLBC_PrintLine("  Part 3 value(as sint64): %lld", packet.GetHeaderPartAsSInt64(3));
            LLBC_PrintLine("  Part 4 value(as sint16): %d", packet.GetHeaderPartAsSInt16(4));
            LLBC_PrintLine("  Part 4 value(as sint32): %d", packet.GetHeaderPartAsSInt32(4));
            LLBC_PrintLine("  Part 4 value(as sint64): %lld", packet.GetHeaderPartAsSInt64(4));
            LLBC_PrintLine("  Part 5 value(as float): %f", packet.GetHeaderPartAsFloat(5));
            LLBC_PrintLine("  Part 5 value(as double): %f", packet.GetHeaderPartAsDouble(5));
            LLBC_PrintLine("  Part 6 value(as float): %f", packet.GetHeaderPartAsFloat(6));
            LLBC_PrintLine("  Part 6 value(as double): %f", packet.GetHeaderPartAsDouble(6));
        }

    private:
        bool _asClient;
        time_t _lastUpdateTime;
    };
}

TestCase_Comm_CustomHeaderSvc::TestCase_Comm_CustomHeaderSvc()
: _asClient(false)

, _ip("0.0.0.0")
, _port(8888)

, _svc(LLBC_IService::Create(LLBC_IService::Normal, "CustomHeaderSvcTest"))
{
}

TestCase_Comm_CustomHeaderSvc::~TestCase_Comm_CustomHeaderSvc()
{
    LLBC_Delete(_svc);
}

int TestCase_Comm_CustomHeaderSvc::Run(int argc, char *argv[])
{
    LLBC_PrintLine("comm/CustomHeaderSvc test:");
    if (!ParseArgs(argc, argv))
        return LLBC_FAILED;

    DesignHeader();
    if (!StartUpService())
        return LLBC_FAILED;

    LLBC_PrintLine("Service start up succeed, %s endPoint: %s:%d", 
        _asClient ? "client" : "server", _ip.c_str(), _port);

    LLBC_PrintLine("Press any key to exit...");
    getchar();

    return LLBC_OK;
}

bool TestCase_Comm_CustomHeaderSvc::ParseArgs(int argc, char *argv[])
{
    if (argc < 4)
    {
        LLBC_FilePrintLine(stderr, "argument error, eg: ./a [client/server] ip port");
        return false;
    }

    _ip = LLBC_String(argv[2]).strip();
    _port = LLBC_Str2Int32(argv[3]);

    _asClient = LLBC_String(argv[1]).strip().tolower() == "client" ? true : false;

    return true;
}

void TestCase_Comm_CustomHeaderSvc::DesignHeader()
{
    int serialNo = 0;

    LLBC_PacketHeaderDesc *headerDesc = LLBC_New(LLBC_PacketHeaderDesc);
    // Len part
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++) // 0
        .SetPartLen(4)
        .SetIsLenPart(true)
        .Done();

    // Opcode part
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++) // 1
        .SetPartLen(2)
        .SetIsOpcodePart(true)
        .Done();

    // Custom part: use to store string: llbc
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++) // 2
        .SetPartLen(10)
        .Done();

    // Custom part2: use to store sint64 value: 888888888888
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++) // 3
        .SetPartLen(sizeof(sint64))
        .SetIsLenIncludedSelf(false)
        .Done();

    // Custom part3: use to store sint32 value.
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++) // 4
        .SetPartLen(sizeof(sint32))
        .SetIsLenIncludedSelf(false)
        .Done();

    // Custom part4: use to store double value.
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++) // 5
        .SetPartLen(sizeof(double))
        .Done();

    // Custom part5: use to store float value.
    headerDesc->AddPartDesc()
        .SetSerialNo(serialNo++) // 6
        .SetPartLen(sizeof(float))
        .Done();

    // Set to llbc framework.
    LLBC_IService::SetPacketHeaderDesc(headerDesc);
}

bool TestCase_Comm_CustomHeaderSvc::StartUpService()
{
    Facade *facade = LLBC_New1(Facade, _asClient);
    _svc->RegisterFacade(facade);
    if (_asClient)
        _svc->Subscribe(OPCODE, facade, &Facade::OnRecv);

    _svc->Start();

    if (!_asClient)
    {
        if (_svc->Listen(_ip.c_str(), _port) == 0)
        {
            LLBC_FilePrintLine(stderr, 
                "Listen in %s:%d failed, error: %s", _ip.c_str(), _port, LLBC_FormatLastError());
            return false;
        }
    }
    else
    {
        if (_svc->Connect(_ip.c_str(), _port) == 0)
        {
            LLBC_FilePrintLine(stderr,
                "Connect to %s:%d failed, error: %s", _ip.c_str(), _port, LLBC_FormatLastError());
            return false;
        }
    }

    return true;
}
