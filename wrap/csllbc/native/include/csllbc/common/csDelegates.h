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

#pragma once

#include "csllbc/common/Macro.h"

/**
 * \brief All csharp delegates encapsulation.
 *        Note: All delegate name must be same with csharp layer delegate define.
 */
class LLBC_HIDDEN csllbc_Delegates
{
public:
    ///////////////////////////////////////////////////////////////////////////////////
    //
    // Component about delegates define:
    //
    typedef void (__stdcall *Deleg_Comp_OnInit)();
    typedef void (__stdcall *Deleg_Comp_OnDestroy)();
    typedef void (__stdcall *Deleg_Comp_OnStart)();
    typedef void (__stdcall *Deleg_Comp_OnStop)();

    typedef void (__stdcall *Deleg_Comp_OnUpdate)();
    typedef void (__stdcall *Deleg_Comp_OnIdle)(int idleTime);

    typedef void (__stdcall *Deleg_Comp_OnSessionCreate)(bool isListen,
                                                         int sessionId,
                                                         int acceptSessionId,
                                                         int socketHandle,
                                                         const char *localHost,
                                                         int localHostLen,
                                                         int localHostPort,
                                                         const char *remoteHost,
                                                         int remoteHostLen,
                                                         int remoteHostPort);
    typedef void (__stdcall *Deleg_Comp_OnSessionDestroy)(bool isListen,
                                                          int sessionId,
                                                          int acceptSessionId,
                                                          int socketHandle,
                                                          const char *localHost,
                                                          int localHostLen,
                                                          int localPort,
                                                          const char *remoteHost,
                                                          int remoteHostLen,
                                                          int remotePort,
                                                          bool fromSvc,
                                                          const char *reason,
                                                          int reasonLen,
                                                          int errNo,
                                                          int subErrNo);
    typedef void (__stdcall *Deleg_Comp_OnAsyncConnResult)(bool connected,
                                                           const char *reason,
                                                           int reasonLen,
                                                           const char *remoteHost,
                                                           int remoteHostLen,
                                                           int remotePort);

    typedef void (__stdcall *Deleg_Comp_OnProtoReport)(int sessionId,
                                                       int layer,
                                                       int level,
                                                       const char *report,
                                                       int reportLen);
    typedef void (__stdcall *Deleg_Comp_OnUnHandledPacket)(int sessionId,
                                                           int opcode,
                                                           void *data,
                                                           int dataLen,
                                                           int status);
    ///////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////
    //
    // Service about delegates.
    //
    typedef void *(__stdcall *Deleg_Service_EncodePacket)(int sessionId,
                                                          sint64 packetId,
                                                          int *encodedSucceed,
                                                          int *encodedLength,
                                                          int *errMsgLength);
    typedef void *(__stdcall *Deleg_Service_DecodePacket)(int sessionId,
                                                          int opcode,
                                                          const void *data,
                                                          int dataLen,
                                                          int status,
                                                          int *errMsgLength);
    typedef void (__stdcall *Deleg_Service_PacketHandler)(int sessionId,
                                                          int opcode,
                                                          const void *data);
    typedef int (__stdcall *Deleg_Service_PacketPreHandler)(int sessionId,
                                                            int opcode,
                                                            const void *data);
    typedef int (__stdcall *Deleg_Service_PacketUnifyPreHandler)(int sessionId,
                                                                 int opcode,
                                                                 const void *data);
    typedef void (__stdcall *Deleg_Service_NativeCouldNotFoundDecoderReport)(int sessionId,
                                                                             int opcode,
                                                                             const void *data,
                                                                             int dataLen,
                                                                             int status);
    ///////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////////
    //
    // Timer about delegates.
    //
    typedef int (__stdcall *Deleg_Timer_OnTimeout)();
    typedef void (__stdcall *Deleg_Timer_OnCancel)();
    ///////////////////////////////////////////////////////////////////////////////////
};


