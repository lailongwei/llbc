/**
 * @file    _Service.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */

#include "csllbc/common/Export.h"

#include "csllbc/comm/csCoder.h"
#include "csllbc/comm/csFacade.h"
#include "csllbc/comm/csService.h"

#include "csllbc/comm/_Service.h"

LLBC_BEGIN_C_DECL

csllbc_Service *csllbc_Service_Create(int svcType,
                                      const char *svcName,
                                      csllbc_Delegates::Deleg_Service_EncodePacket encodeDeleg,
                                      csllbc_Delegates::Deleg_Service_DecodePacket decodeDeleg,
                                      csllbc_Delegates::Deleg_Service_PacketHandler handlerDeleg,
                                      csllbc_Delegates::Deleg_Service_PacketPreHandler preHandlerDeleg,
                                      csllbc_Delegates::Deleg_Service_PacketUnifyPreHandler unifyPreHandlerDeleg,
                                      csllbc_Delegates::Deleg_Service_NativeCouldNotFoundDecoderReport notFoundDecoderDeleg)
{
    if (svcType != static_cast<int>(LLBC_IService::Raw) &&
        svcType != static_cast<int>(LLBC_IService::Normal))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return NULL;
    }

    return LLBC_New8(csllbc_Service,
                     static_cast<csllbc_Service::Type>(svcType),
                     svcName,
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

int csllbc_Service_GetType(csllbc_Service *svc)
{
    return static_cast<int>(svc->GetType());
}

int csllbc_Service_GetId(csllbc_Service *svc)
{
    return svc->GetId();
}

int csllbc_Service_GetDriveMode(csllbc_Service *svc)
{
    return static_cast<int>(svc->GetDriveMode());
}

int csllbc_Service_SetDriveMode(csllbc_Service *svc, int driveMode)
{
    return svc->SetDriveMode(static_cast<LLBC_IService::DriveMode>(driveMode));
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
        nativeReason = NULL;
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
    csllbc_Coder *coder = LLBC_New(csllbc_Coder);
    coder->SetEncodeInfo(packetId, svc->GetEncodePacketDeleg());

    if (svc->Send(sessionId, opcode, coder, status) != LLBC_OK)
    {
        LLBC_Delete(coder);
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
    for (int idx = 0; idx < sessionIdCount; idx++)
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

int csllbc_Service_RegisterFacade(csllbc_Service *svc,
                                  csllbc_Delegates::Deleg_Facade_OnInit initDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnDestroy destroyDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnStart startDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnStop stopDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnUpdate updateDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnIdle idleDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnSessionCreate sessionCreateDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnSessionDestroy sessionDestroyDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnAsyncConnResult asyncConnResultDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnProtoReport protoReportDeleg,
                                  csllbc_Delegates::Deleg_Facade_OnUnHandledPacket unHandledPacketDeleg)
{
    csllbc_Facade *facade = new csllbc_Facade(initDeleg, destroyDeleg,
                                              startDeleg, stopDeleg,
                                              updateDeleg, idleDeleg,
                                              sessionCreateDeleg, sessionDestroyDeleg, asyncConnResultDeleg,
                                              protoReportDeleg, unHandledPacketDeleg);

    if (svc->RegisterFacade(facade) != LLBC_OK)
    {
        LLBC_Delete(facade);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int csllbc_Service_RegisterCoder(csllbc_Service *svc, int opcode)
{
    return svc->RegisterCoder(opcode);
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
