/**
 * @file    Comm.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/09
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/comm/PyTimerScheduler.h"
#include "pyllbc/comm/Comm.h"

#include "src/comm/_CommMethods.h"

void pyllbc_InitComm()
{
    pyllbc_s_TimerScheduler->Init();
}

void pyllbc_DestroyComm()
{
}

void pyllbc_AddCommMethods()
{
}

void pyllbc_AddCommObjs()
{
    pyllbc_CommMethods &methods = *pyllbc_s_CommMethods;

    pyllbc_Module *topMod = pyllbc_s_TopModule;

    // Create llbc.codec module.
    pyllbc_Module *codecMod = new pyllbc_Module("codec", topMod);
    codecMod->AddMethod(methods.encode);
    codecMod->AddMethod(methods.decode);

    pyllbc_Module *inlMod = pyllbc_InlModule;
    // Add service types to inl module.
    inlMod->AddObject("SVC_TYPE_NORMAL", LLBC_IService::Normal);
    inlMod->AddObject("SVC_TYPE_RAW", LLBC_IService::Raw);
    // Add codec strategies to inl module.
    inlMod->AddObject("SVC_CODEC_JSON", pyllbc_Service::JsonCodec);
    inlMod->AddObject("SVC_CODEC_BINARY", pyllbc_Service::BinaryCodec);
    // Add FPS limit value to inl module.
    inlMod->AddObject("SVC_MIN_FPS", LLBC_CFG_COMM_MIN_SERVICE_FPS);
    inlMod->AddObject("SVC_MAX_FPS", LLBC_CFG_COMM_MAX_SERVICE_FPS);
    // Add per-thread drive services limit value to inl module.
    inlMod->AddObject("SVC_MAX_COUNT", LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT);

    // Add all timer about methods to inl module.
    inlMod->AddMethod(methods.NewPyTimer);
    inlMod->AddMethod(methods.DelPyTimer);
    inlMod->AddMethod(methods.PyTimerIsIgnoredDeadRef);
    inlMod->AddMethod(methods.PyTimerSetIgnoredDeadRef);
    inlMod->AddMethod(methods.PyTimerGetDueTime);
    inlMod->AddMethod(methods.PyTimerGetPeriod);
    inlMod->AddMethod(methods.PyTimerGetTimerId);
    inlMod->AddMethod(methods.PyTimerSchedule);
    inlMod->AddMethod(methods.PyTimerCancel);
    inlMod->AddMethod(methods.PyTimerIsScheduling);
    inlMod->AddMethod(methods.PyTimerIsTimeouting);
    inlMod->AddMethod(methods.PyTimerIsCancelling);
    inlMod->AddMethod(methods.PyTimerUpdateAllTimers);

    // Add packet header part describe about methods/variables to inl module.
    inlMod->AddObject("PacketHeaderPartDesc_MaxPartLen",
            static_cast<sint32>(LLBC_PacketHeaderPartDesc::MaxPartLen));
    inlMod->AddMethod(methods.PacketHeaderPartDesc_New);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_Del);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_GetSerialNo);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetSerialNo);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_IsLenPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetIsLenPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_IsLenIncludedSelf);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetIsLenIncludedSelf);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_GetPartLen);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetPartLen);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_IsOpcodePart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetIsOpcodePart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_IsStatusPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetIsStatusPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_IsServiceIdPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetIsServiceIdPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_IsFlagsPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_SetIsFlagsPart);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_IsValid);
    inlMod->AddMethod(methods.PacketHeaderPartDesc_ToString);

    // Add packet header desc about methods & variables to inl module.
    inlMod->AddObject("PacketHeaderDesc_MaxHeaderLen",
            static_cast<sint32>(LLBC_PacketHeaderDesc::MaxHeaderLen));
    inlMod->AddMethod(methods.PacketHeaderDesc_New);
    inlMod->AddMethod(methods.PacketHeaderDesc_Del);
    inlMod->AddMethod(methods.PacketHeaderDesc_AddPart);
    inlMod->AddMethod(methods.PacketHeaderDesc_RemovePart);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetHeaderLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetLenPartIncludedLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetLenPartNotIncludedLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetLenPartLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetLenPartOffset);
    inlMod->AddMethod(methods.PacketHeaderDesc_IsHasOpcodePart);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetOpcodePartLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetOpcodePartOffset);
    inlMod->AddMethod(methods.PacketHeaderDesc_IsHasStatusPart);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetStatusPartLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetStatusPartOffset);
    inlMod->AddMethod(methods.PacketHeaderDesc_IsHasServiceIdPart);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetServiceIdPartLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetServiceIdPartOffset);
    inlMod->AddMethod(methods.PacketHeaderDesc_IsHasFlagsPart);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetFlagsPartLen);
    inlMod->AddMethod(methods.PacketHeaderDesc_GetFlagsPartOffset);
    inlMod->AddMethod(methods.PacketHeaderDesc_ToString);

    // Add packet about methods to inl module.
    inlMod->AddMethod(methods.Packet_GetHeaderPartAsSInt64);
    inlMod->AddMethod(methods.Packet_GetHeaderPartAsDouble);
    inlMod->AddMethod(methods.Packet_GetHeaderPartAsStr);

    inlMod->AddMethod(methods.Packet_SetHeaderPartAsSInt64);
    inlMod->AddMethod(methods.Packet_SetHeaderPartAsDouble);
    inlMod->AddMethod(methods.Packet_SetHeaderPartAsStr);

    // Add all service about methods to inl module.
    inlMod->AddMethod(methods.NewService);
    inlMod->AddMethod(methods.DelService);
    inlMod->AddMethod(methods.SetPacketHeaderDesc);
    inlMod->AddMethod(methods.GetServiceType);
    inlMod->AddMethod(methods.GetServiceTypeStr);
    inlMod->AddMethod(methods.GetServiceFPS);
    inlMod->AddMethod(methods.SetServiceFPS);
    inlMod->AddMethod(methods.GetServiceFrameInterval);
    inlMod->AddMethod(methods.SuppressServiceCoderNotFoundWarning);
    inlMod->AddMethod(methods.StartService);
    inlMod->AddMethod(methods.StopService);
    inlMod->AddMethod(methods.RegisterFacade);
    inlMod->AddMethod(methods.RegisterCodec);
    inlMod->AddMethod(methods.Listen);
    inlMod->AddMethod(methods.Connect);
    inlMod->AddMethod(methods.AsyncConn);
    inlMod->AddMethod(methods.RemoveSession);
    inlMod->AddMethod(methods.SendData);
    inlMod->AddMethod(methods.Multicast);
    inlMod->AddMethod(methods.Broadcast);
    inlMod->AddMethod(methods.Subscribe);
    inlMod->AddMethod(methods.PreSubscribe);
    inlMod->AddMethod(methods.UnifyPreSubscribe);
    inlMod->AddMethod(methods.GetServiceCodec);
    inlMod->AddMethod(methods.SetServiceCodec);
    inlMod->AddMethod(methods.InstallErrHooker);
    inlMod->AddMethod(methods.UninstallErrHooker);
    inlMod->AddMethod(methods.ClearHookedErrors);
    inlMod->AddMethod(methods.Post);
    inlMod->AddMethod(methods.ServiceMainLoop);

    // Add Obj<-->Json coder helper to top module.
    topMod->AddMethod(methods.j2o);
    topMod->AddMethod(methods.o2j);

    // Create protocol sub module.
    pyllbc_Module *protoMod = LLBC_New2(pyllbc_Module, "Protocol", topMod);
    // Add layer enumerations to protocol module.
    protoMod->AddObject("PACK_LAYER", LLBC_ProtocolLayer::PackLayer);
    protoMod->AddObject("COMPRESS_LAYER", LLBC_ProtocolLayer::CompressLayer);
    protoMod->AddObject("CODEC_LAYER", LLBC_ProtocolLayer::CodecLayer);

    // Add protocol report level enumerations to protocol module.
    protoMod->AddObject("DEBUG", LLBC_ProtoReportLevel::Debug);
    protoMod->AddObject("INFO", LLBC_ProtoReportLevel::Info);
    protoMod->AddObject("WARN", LLBC_ProtoReportLevel::Warn);
    protoMod->AddObject("ERROR", LLBC_ProtoReportLevel::Error);

    // Add layer2str, level2str method to protocol module.
    protoMod->AddMethod(methods.layer2str);
    protoMod->AddMethod(methods.level2str);
}
