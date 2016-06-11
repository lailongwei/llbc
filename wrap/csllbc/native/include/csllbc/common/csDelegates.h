/**
 * @file    csDelegates.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/22
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_COM_CSDELEGATES_H__
#define __CSLLBC_COM_CSDELEGATES_H__

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
    // Facade about delegates define:
    //
    typedef void (__stdcall *Deleg_Facade_OnInit)();
    typedef void (__stdcall *Deleg_Facade_OnDestroy)();
    typedef void (__stdcall *Deleg_Facade_OnStart)();
    typedef void (__stdcall *Deleg_Facade_OnStop)();

    typedef void (__stdcall *Deleg_Facade_OnUpdate)();
    typedef void (__stdcall *Deleg_Facade_OnIdle)(int idleTime);

    typedef void (__stdcall *Deleg_Facade_OnSessionCreate)(int sessionId,
                                                           int socketHandle,
                                                           bool isListen,
                                                           const char *localHost,
                                                           int localHostLen,
                                                           int localHostPort,
                                                           const char *remoteHost,
                                                           int remoteHostLen,
                                                           int remoteHostPort);
    typedef void (__stdcall *Deleg_Facade_OnSessionDestroy)(int sessionId,
                                                            int socketHandle,
                                                            bool isListen,
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
    typedef void (__stdcall *Deleg_Facade_OnAsyncConnResult)(bool connected,
                                                             const char *reason,
                                                             int reasonLen,
                                                             const char *remoteHost,
                                                             int remoteHostLen,
                                                             int remotePort);

    typedef void (__stdcall *Deleg_Facade_OnProtoReport)(int sessionId,
                                                         int layer,
                                                         int level,
                                                         const char *report,
                                                         int reportLen);
    typedef void (__stdcall *Deleg_Facade_OnUnHandledPacket)(int sessionId, int opcode, void *data, int dataLen, int status);
    ///////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////
    //
    // Service about delegates.
    //
    typedef void *(__stdcall *Deleg_Service_EncodePacket)(int sessionId, sint64 packetId, int *encodedSucceed, int *encodedLength, int *errMsgLength);
    typedef void *(__stdcall *Deleg_Service_DecodePacket)(int sessionId, int opcode, const void *data, int dataLen, int status, int *errMsgLength);
    typedef void (__stdcall *Deleg_Service_PacketHandler)(int sessionId, int opcode, const void *data);
    typedef int (__stdcall *Deleg_Service_PacketPreHandler)(int sessionId, int opcode, const void *data);
    typedef int (__stdcall *Deleg_Service_PacketUnifyPreHandler)(int sessionId, int opcode, const void *data);
    typedef void (__stdcall *Deleg_Service_NativeCouldNotFoundDecoderReport)(int sessionId, int opcode, const void *data, int dataLen, int status);
    ///////////////////////////////////////////////////////////////////////////////////
};

#endif // __CSLLBC_COM_CSDeleg_FacadeATES_H__
