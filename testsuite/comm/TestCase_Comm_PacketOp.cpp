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


#include "comm/TestCase_Comm_PacketOp.h"

namespace
{
    void __DeletePreHandleResult(void *result)
    {
        delete reinterpret_cast<char *>(result);
        std::cout <<"Prehandle result data deleted!" <<std::endl;
    }
}

TestCase_Comm_PacketOp::TestCase_Comm_PacketOp()
{
}

TestCase_Comm_PacketOp::~TestCase_Comm_PacketOp()
{
}

int TestCase_Comm_PacketOp::Run(int argc, char *argv[])
{
    LLBC_PrintLn("llbc library communication/packet test:");

    // Raw data type read/write test.
    std::cout <<"Raw data type read/write test(net byte order flag: " 
        <<(LLBC_CFG_COMM_ORDER_IS_NET_ORDER ? "TRUE" : "FALSE") <<"):" <<std::endl;
    bool boolVal = true;
    char charVal = 'X';
    sint8 sint8Val = -8;
    uint8 uint8Val = 8;
    sint16 sint16Val = -16;
    uint16 uint16Val = 16;
    sint32 sint32Val = -32;
    uint32 uint32Val = 32;
    sint64 sint64Val = -64;
    uint64 uint64Val = 64;
    long longVal = -1;
    LLBC_NS ulong ulongVal = 1;
    float floatVal = 1.01f;
    double doubleVal = 2.02;

    LLBC_Packet packet;
    packet <<boolVal <<charVal <<sint8Val <<uint8Val 
        <<sint16Val <<uint16Val <<sint32Val <<uint32Val
        <<sint64Val <<uint64Val <<longVal <<ulongVal
        <<floatVal <<doubleVal;
    packet >>boolVal >>charVal >>sint8Val >>uint8Val
        >>sint16Val >>uint16Val >>sint32Val >>uint32Val
        >>sint64Val >>uint64Val >>longVal >>ulongVal
        >>floatVal >>doubleVal;
    std::cout <<"Packet::Read(): boolVal: " <<boolVal <<", charVal: " <<charVal
        <<", sint8Val: " <<static_cast<sint32>(sint8Val) <<", uint8Val: " <<static_cast<uint32>(uint8Val)
        <<", sint16Val: " <<sint16Val <<", uint16Val: " <<uint16Val
        <<", sint32Val: " <<sint32Val <<", uint32Val: " <<uint32Val
        <<", sint64Val: " <<sint64Val <<", uint64Val: " <<uint64Val
        <<", longVal: " <<longVal <<", ulongVal: " <<ulongVal
        <<", floatVal: " <<floatVal <<", doubleVal: " <<doubleVal
        <<std::endl;

    // std::string/LLBC_String test.
    std::cout <<"std::string/LLBC_String read/write test:" <<std::endl;
    std::string stdStr = "Hello, This is a std::string object";
    LLBC_String llbcStr = "Hello, This is a LLBC_String object";

    packet <<stdStr <<llbcStr >>stdStr >>llbcStr;
    std::cout <<"Packet::Read(): std::string: " <<stdStr <<std::endl;
    std::cout <<"Packet::Read(): LLBC_String: " <<llbcStr <<std::endl;

    // STL containers test.
    std::cout <<"STL container test: " <<std::endl;
    std::vector<std::list<LLBC_String> > stlCont;
    stlCont.push_back(std::list<LLBC_String>());
    stlCont.push_back(std::list<LLBC_String>());
    stlCont[0].push_back("World");
    stlCont[0].push_front("Primer");
    stlCont[1].push_back("Foot");
    stlCont[1].push_front("Service Entry");
    packet <<stlCont;
    stlCont.clear();
    packet >>stlCont;
    std::cout <<"Packet::Read(): std::vector<std::list<LLBC_String> >, size = " <<stlCont.size() <<std::endl;
    for(size_t i = 0; i < stlCont.size(); ++i)
    {
        std::cout <<"  elem[ " <<i <<"] value: " <<std::endl;
        std::list<LLBC_String>::const_iterator it = stlCont[i].begin();
        for(int j = 0; it != stlCont[i].end(); ++it, ++j)
        {
            std::cout <<"    elem[" <<i <<"][" <<j <<"] value: " <<*it <<std::endl;
        }
    }

    // Base methods test.
    std::cout <<"\nBase methods test:" <<std::endl;
    LLBC_Packet packet2;
    packet2.SetOpcode(10086);
    packet2.SetStatus(-1);
    packet2.SetSenderServiceId(110);
    packet2.SetRecverServiceId(220);
    packet2.SetFlags(1111);
    std::cout <<"After set opcode/status/serviceId/flags, these values are:" <<std::endl;
    std::cout <<"  opcode: " <<packet2.GetOpcode() <<std::endl;
    std::cout <<"  status: " <<packet2.GetStatus() <<std::endl;
    std::cout <<"  sender service Id: " <<packet2.GetSenderServiceId() <<std::endl;
    std::cout <<"  receiver service Id: " <<packet2.GetRecverServiceId() <<std::endl;

    std::cout <<"  flags: " <<packet2.GetFlags() <<std::endl;
    std::cout <<"  packet length: " <<packet2.GetLength() <<std::endl;
    std::cout <<"  payload length: " <<packet2.GetPayloadLength() <<std::endl;

    // PreHandleResult about test.
    std::cout <<"\nPreHandle result about test:" <<std::endl;
    std::cout <<"Create new packet to test prehandle result clear callback method" <<std::endl;
    LLBC_Packet *preHandleTestPacket = new LLBC_Packet;
    preHandleTestPacket->SetPreHandleResult(new char, __DeletePreHandleResult);

    std::cout <<"Delete prehandle test packet" <<std::endl;
    LLBC_XDelete(preHandleTestPacket);

    // Stream output test.
    std::cout <<"\nPacket stream output test:" <<std::endl;
    LLBC_Packet streamOutputTest;
    streamOutputTest.SetSessionId(10086);
    streamOutputTest.SetSenderServiceId(100);
    streamOutputTest.SetRecverServiceId(200);
    streamOutputTest.SetOpcode(300);
    streamOutputTest.SetStatus(400);
    streamOutputTest.SetPayload(new LLBC_MessageBlock);
    streamOutputTest <<3;
    streamOutputTest <<"Hello world";
    std::cout <<"streamOutputTest::operator<<(std::ostream &): " <<streamOutputTest <<std::endl;

    std::cout <<"Press any key to continue ...";
    getchar();

    return 0;
}
