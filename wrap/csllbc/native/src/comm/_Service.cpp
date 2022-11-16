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

#include "csllbc/common/Export.h"

#include "csllbc/comm/csCoder.h"
#include "csllbc/comm/csComponent.h"
#include "csllbc/comm/csService.h"

#include "csllbc/comm/_Service.h"

LLBC_BEGIN_C_DECL

csllbc_Service *csllbc_Service_Create(const char *svcName,
                                      bool useNormalProtocolFactory,
                                      bool fullStack,
                                      csllbc_Delegates::Deleg_Service_EncodePacket encodeDeleg,
                                      csllbc_Delegates::Deleg_Service_DecodePacket decodeDeleg,
                                      csllbc_Delegates::Deleg_Service_PacketHandler handlerDeleg,
                                      csllbc_Delegates::Deleg_Service_PacketPreHandler preHandlerDeleg,
                                      csllbc_Delegates::Deleg_Service_PacketUnifyPreHandler unifyPreHandlerDeleg,
                                      csllbc_Delegates::Deleg_Service_NativeCouldNotFoundDecoderReport notFoundDecoderDeleg)
{
    return new csllbc_Service(
                    svcName,
                    useNormalProtocolFactory,
                    fullStack,
                    encodeDeleg,
                    decodeDeleg,
                    handlerDeleg,
                    preHandlerDeleg,
                    unifyPreHandlerDeleg,
                    notFoundDecoderDeleg);
}

void csllbc_Service_Delete(csllbc_Service *svc)
{
    LLBC_XDelete(svc);
}

int csllbc_Service_GetId(csllbc_Service *svc)
{
    return svc->GetId();
}

int csllbc_Service_IsFullStack(csllbc_Service *svc)
{
    return svc->IsFullStack() ? 1 : 0;
}

int csllbc_Service_GetDriveMode(csllbc_Service *svc)
{
    return static_cast<int>(svc->GetDriveMode());
}

int csllbc_Service_SetDriveMode(csllbc_Service *svc, int driveMode)
{
    return svc->SetDriveMode(static_cast<LLBC_Service::DriveMode>(driveMode));
}

int csllbc_Service_Start(csllbc_Service *svc, int pollerCount)
{
    return svc->Start(pollerCount);
}

void csllbc_Service_Stop(csllbc_Service *svc)
{
    svc->Stop();
}

int csllbc_Service_IsStarted(csllbc_Service *svc)
{
    return svc->IsStarted() ? 1 : 0;
}

int csllbc_Service_GetFPS(csllbc_Service *svc)
{
    return svc->GetFPS();
}

int csllbc_Service_SetFPS(csllbc_Service *svc, int fps)
{
    return svc->SetFPS(fps);
}

int csllbc_Service_GetFrameInterval(csllbc_Service *svc)
{
    return svc->GetFrameInterval();
}

int csllbc_Service_Listen(csllbc_Service *svc, const char *ip, int port)
{
    return svc->Listen(ip, port);
}

int csllbc_Service_Connect(csllbc_Service *svc, const char *ip, int port)
{
    return svc->Connect(ip, port);
}

int csllbc_Service_AsyncConn(csllbc_Service *svc, const char *ip, int port)
{
    return svc->AsyncConn(ip, port);
}

int csllbc_Service_RemoveSession(csllbc_Service *svc, int sessionId, const char *reason, int reasonLength)
{
    char *nativeReason;
    if (reasonLength == 0)
    {
        nativeReason = nullptr;
    }
    else
    {
        nativeReason = LLBC_Malloc(char, reasonLength + 1);
        memcpy(nativeReason, reason, reasonLength);
        nativeReason[reasonLength] = '\0';
    }

    const int ret = svc->RemoveSession(sessionId, nativeReason);
    LLBC_XFree(nativeReason);

    return ret;
}

int csllbc_Service_IsSessionValidate(csllbc_Service *svc, int sessionId)
{
    return svc->IsSessionValidate(sessionId) ? 1 : 0;
}

int csllbc_Service_SendBytes(csllbc_Service *svc,
                             int sessionId,
                             int opcode,
                             const void *data,
                             int dataLen,
                             int status)
{
    return svc->Send(sessionId, opcode, data, static_cast<size_t>(dataLen), status);
}

int csllbc_Service_SendPacket(csllbc_Service *svc,
                              int sessionId,
                              int opcode,
                              sint64 packetId,
                              int status)
{
    csllbc_Coder *coder = new csllbc_Coder;
    coder->SetEncodeInfo(packetId, svc->GetEncodePacketDeleg());

    if (svc->Send(sessionId, opcode, coder, status) != LLBC_OK)
    {
        delete coder;
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int csllbc_Service_Multicast(csllbc_Service *svc,
                             int *sessionIds,
                             int sessionIdCount,
                             int opcode,
                             const void *data,
                             int dataLen,
                             int status)
{
    LLBC_SessionIdList sessionIdList(sessionIdCount);
    for (int idx = 0; idx < sessionIdCount; ++idx)
        sessionIdList.push_back(sessionIds[idx]);

    LLBC_XFree(sessionIds);
    
    return svc->Multicast(sessionIdList, opcode, data, static_cast<size_t>(dataLen), status);
}

int csllbc_Service_Broadcast(csllbc_Service *svc,
                             int opcode,
                             const void *data,
                             int dataLen,
                             int status)
{
    return svc->Broadcast(opcode, data, static_cast<size_t>(dataLen), status);
}

int csllbc_Service_AddComponent(csllbc_Service *svc,
                                csllbc_Delegates::Deleg_Comp_OnInit initDeleg,
                                csllbc_Delegates::Deleg_Comp_OnDestroy destroyDeleg,
                                csllbc_Delegates::Deleg_Comp_OnStart startDeleg,
                                csllbc_Delegates::Deleg_Comp_OnStop stopDeleg,
                                csllbc_Delegates::Deleg_Comp_OnUpdate updateDeleg,
                                csllbc_Delegates::Deleg_Comp_OnIdle idleDeleg,
                                csllbc_Delegates::Deleg_Comp_OnSessionCreate sessionCreateDeleg,
                                csllbc_Delegates::Deleg_Comp_OnSessionDestroy sessionDestroyDeleg,
                                csllbc_Delegates::Deleg_Comp_OnAsyncConnResult asyncConnResultDeleg,
                                csllbc_Delegates::Deleg_Comp_OnProtoReport protoReportDeleg,
                                csllbc_Delegates::Deleg_Comp_OnUnHandledPacket unHandledPacketDeleg)
{
    csllbc_Component *comp = new csllbc_Component(initDeleg, destroyDeleg,
                                                  startDeleg, stopDeleg,
                                                  updateDeleg, idleDeleg,
                                                  sessionCreateDeleg, sessionDestroyDeleg, asyncConnResultDeleg,
                                                  protoReportDeleg, unHandledPacketDeleg);

    if (svc->AddComponent(comp) != LLBC_OK)
    {
        delete comp;
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int csllbc_Service_AddCoder(csllbc_Service *svc, int opcode)
{
    return svc->AddCoder(opcode);
}

int csllbc_Service_Subscribe(csllbc_Service *svc, int opcode)
{
    return svc->Subscribe(opcode);
}

int csllbc_Service_PreSubscribe(csllbc_Service *svc, int opcode)
{
    return svc->PreSubscribe(opcode);
}

int csllbc_Service_UnifyPreSubscribe(csllbc_Service *svc)
{
    return svc->UnifyPreSubscribe();
}

void csllbc_Service_OnSvc(csllbc_Service *svc, bool fullFrame)
{
    svc->OnSvc(fullFrame);
}

LLBC_END_C_DECL
