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
    inlMod->AddObject("SVC_TYPE_CUSTOM", LLBC_IService::Custom);
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

    // Add all service about methods to inl module.
    inlMod->AddMethod(methods.NewService);
    inlMod->AddMethod(methods.DelService);
    inlMod->AddMethod(methods.GetServiceId);
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
