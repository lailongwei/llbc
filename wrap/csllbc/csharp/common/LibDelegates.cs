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


using System;

namespace llbc
{
    /// <summary>
    /// All llbc library delegates define.
    /// </summary>
    internal partial class LLBCNative
    {
        /////////////////////////////////////////////////////////////////////////////////////
        //
        // All component delegates define.
        //
        public delegate void Deleg_Comp_OnInit();
        public delegate void Deleg_Comp_OnDestroy();
        public delegate void Deleg_Comp_OnStart();
        public delegate void Deleg_Comp_OnStop();

        public delegate void Deleg_Comp_OnUpdate();
        public delegate void Deleg_Comp_OnIdle(int idleTime);

        public delegate void Deleg_Comp_OnSessionCreate(bool isListen,
                                                        int sessionId,
                                                        int acceptSessionId,
                                                        int socketHandle,
                                                        IntPtr localHost,
                                                        int localHostLen,
                                                        int localPort,
                                                        IntPtr remoteHost,
                                                        int remoteHostLen,
                                                        int remotePort);
        public delegate void Deleg_Comp_OnSessionDestroy(bool isListen,
                                                         int sessionId,
                                                         int acceptSessionId,
                                                         int socketHandle,
                                                         IntPtr localHost,
                                                         int localHostLen,
                                                         int localPort,
                                                         IntPtr remoteHost,
                                                         int remoteHostLen,
                                                         int remotePort,
                                                         bool fromSvc,
                                                         IntPtr reason,
                                                         int reasonLen,
                                                         int errNo,
                                                         int subErrNo);
        public delegate void Deleg_Comp_OnAsyncConnResult(bool connected,
                                                          IntPtr reason,
                                                          int reasonLen,
                                                          IntPtr remoteHost,
                                                          int remoteHostLen,
                                                          int remotePort);

        public delegate void Deleg_Comp_OnProtoReport(int sessionId,
                                                      int layer,
                                                      int level,
                                                      IntPtr report,
                                                      int reportLen);
        public delegate void Deleg_Comp_OnUnHandledPacket(int sessionId,
                                                          int opcode,
                                                          IntPtr data,
                                                          int dataLen,
                                                          int status);
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        //
        // Service about delegates.
        //
        public delegate IntPtr Deleg_Service_EncodePacket(int sessionId,
                                                          long packetId,
                                                          IntPtr encodedSucceed,
                                                          IntPtr encodedLength,
                                                          IntPtr errMsgLength);
        public delegate IntPtr Deleg_Service_DecodePacket(int sessionId,
                                                          int opcode,
                                                          IntPtr data,
                                                          int dataLen,
                                                          int status,
                                                          IntPtr errMsgLength);
        public delegate void Deleg_Service_PacketHandler(int sessionId,
                                                         int opcode,
                                                         IntPtr data);
        public delegate int Deleg_Service_PacketPreHandler(int sessionId,
                                                           int opcode,
                                                           IntPtr data);
        public delegate int Deleg_Service_PacketUnifyPreHandler(int sessionId,
                                                                int opcode,
                                                                IntPtr data);
        public delegate void Deleg_Service_NativeCouldNotFoundDecoderReport(int sessionId,
                                                                            int opcode,
                                                                            IntPtr data,
                                                                            int dataLen,
                                                                            int status);
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        //
        // Timer about delegates.
        //
        public delegate void Deleg_Timer_OnTimeout();
        public delegate void Deleg_Timer_OnCancel();
        /////////////////////////////////////////////////////////////////////////////////////
    }
}