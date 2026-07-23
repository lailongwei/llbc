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

#include "comm/Example_Comm_Protocol.h"

int Example_Comm_Protocol::Run(int argc, char *argv[])
{
    LLBC_PrintLn("Comm Protocol example: protocol-stack layers and protocol-factory selection");

    // The protocol stack (llbc/include/llbc/comm/protocol/) is roughly split, top to bottom, into several layers:
    //   - Pack     layer: packet splitting/assembly (PacketProtocol), solving TCP sticky/partial packets;
    //   - Compress layer: optional compression (CompressProtocol);
    //   - Codec    layer: encode/decode (CodecProtocol), using a Coder to convert between packet body and business objects;
    // Each layer is created one by one via LLBC_IProtocolFactory::Create(layer), forming an LLBC_ProtocolStack.
    //
    // The library ships two built-in protocol factories:
    //   - LLBC_NormalProtocolFactory: a full opcode-based packet-splitting protocol (default);
    //   - LLBC_RawProtocolFactory   : a raw stream protocol, no packet splitting, opcode fixed to 0.
    // Choose the protocol factory via the second argument of LLBC_Service::Create(name, dftProtocolFactory, fullStack).

    // 1) Use the default (Normal) protocol factory: just pass nullptr.
    LLBC_PrintLn("- Creating a Normal-protocol-stack Service (opcode-based packet framing):");
    LLBC_Service *normalSvc = LLBC_Service::Create("NormalProtoSvc", nullptr);
    if (normalSvc->Start() == LLBC_OK)
    {
        LLBC_PrintLn("  Started, fullStack:%s", normalSvc->IsFullStack() ? "true" : "false");
        LLBC_Sleep(100);
        normalSvc->Stop();
    }
    else
    {
        LLBC_PrintLn("  Failed to start, error:%s", LLBC_FormatLastError());
    }
    delete normalSvc;

    // 2) Explicitly use the Raw protocol factory: suited for custom/raw-stream scenarios.
    //    The protocol factory object is taken over and freed by the Service, so each Service needs its own new instance.
    LLBC_PrintLn("- Creating a Raw-protocol-stack Service (raw stream, opcode always 0):");
    LLBC_Service *rawSvc = LLBC_Service::Create("RawProtoSvc", new LLBC_RawProtocolFactory);
    if (rawSvc->Start() == LLBC_OK)
    {
        LLBC_PrintLn("  Started, fullStack:%s", rawSvc->IsFullStack() ? "true" : "false");
        LLBC_Sleep(100);
        rawSvc->Stop();
    }
    else
    {
        LLBC_PrintLn("  Failed to start, error:%s", LLBC_FormatLastError());
    }
    delete rawSvc;

    // Tip: to customize the protocol, implement LLBC_IProtocolFactory and return custom
    //      LLBC_IProtocol at each layer; you can also pass protoFactory at Listen/Connect to override a single session's protocol stack.
    LLBC_PrintLn("- Note: custom protocols can implement LLBC_IProtocolFactory and override the protocol per session in Listen/Connect.");

    return LLBC_OK;
}
