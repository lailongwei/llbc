/**
 * @file    LibDelegates.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/22
 * @version 1.0
 *
 * @brief
 */

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
        // All facade delegates define.
        //
        public delegate void Deleg_Facade_OnInit();
        public delegate void Deleg_Facade_OnDestroy();
        public delegate void Deleg_Facade_OnStart();
        public delegate void Deleg_Facade_OnStop();

        public delegate void Deleg_Facade_OnUpdate();
        public delegate void Deleg_Facade_OnIdle(int idleTime);

        public delegate void Deleg_Facade_OnSessionCreate(int sessionId,
                                                          int socketHandle,
                                                          bool isListen,
                                                          IntPtr localHost,
                                                          int localHostLen,
                                                          int localPort,
                                                          IntPtr remoteHost,
                                                          int remoteHostLen,
                                                          int remotePort);
        public delegate void Deleg_Facade_OnSessionDestroy(int sessionId,
                                                           int socketHandle,
                                                           bool isListen,
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
        public delegate void Deleg_Facade_OnAsyncConnResult(bool connected,
                                                            IntPtr reason,
                                                            int reasonLen,
                                                            IntPtr remoteHost,
                                                            int remoteHostLen,
                                                            int remotePort);

        public delegate void Deleg_Facade_OnProtoReport(int sessionId,
                                                        int layer,
                                                        int level,
                                                        IntPtr report,
                                                        int reportLen);
        public delegate void Deleg_Facade_OnUnHandledPacket(int sessionId, int opcode, IntPtr data, int dataLen, int status);
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        //
        // Service about delegates.
        //
        public delegate IntPtr Deleg_Service_EncodePacket(int sessionId, long packetId, IntPtr encodedSucceed, IntPtr encodedLength, IntPtr errMsgLength);
        public delegate IntPtr Deleg_Service_DecodePacket(int sessionId, int opcode, IntPtr data, int dataLen, int status, IntPtr errMsgLength);
        public delegate void Deleg_Service_PacketHandler(int sessionId, int opcode, IntPtr data);
        public delegate int Deleg_Service_PacketPreHandler(int sessionId, int opcode, IntPtr data);
        public delegate int Deleg_Service_PacketUnifyPreHandler(int sessionId, int opcode, IntPtr data);
        public delegate void Deleg_Service_NativeCouldNotFoundDecoderReport(int sessionId, int opcode, IntPtr data, int dataLen, int status);
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        //
        // Timer about delegates.
        //
        public delegate int Deleg_Timer_OnTimeout();
        public delegate void Deleg_Timer_OnCancel();
        /////////////////////////////////////////////////////////////////////////////////////
    }
}