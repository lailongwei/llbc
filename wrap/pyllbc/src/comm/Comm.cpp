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

    // Add all packet about methods to inl module.
    inlMod->AddMethod(methods.Packet_GetLocalIp);
    inlMod->AddMethod(methods.Packet_GetLocalPort);
    inlMod->AddMethod(methods.Packet_GetPeerIp);
    inlMod->AddMethod(methods.Packet_GetPeerPort);
    inlMod->AddMethod(methods.Packet_GetStatusDesc);
    inlMod->AddMethod(methods.Packet_GetExtData1);
    inlMod->AddMethod(methods.Packet_GetExtData2);
    inlMod->AddMethod(methods.Packet_GetExtData3);

    // Add all service about methods to inl module.
    inlMod->AddMethod(methods.NewService);
    inlMod->AddMethod(methods.DelService);
    inlMod->AddMethod(methods.GetServiceId);
    inlMod->AddMethod(methods.GetServiceFPS);
    inlMod->AddMethod(methods.SetServiceFPS);
    inlMod->AddMethod(methods.GetServiceFrameInterval);
    inlMod->AddMethod(methods.SuppressServiceCoderNotFoundWarning);
    inlMod->AddMethod(methods.StartService);
    inlMod->AddMethod(methods.StopService);
    inlMod->AddMethod(methods.AddComponent);
    inlMod->AddMethod(methods.AddLibComponent);
    inlMod->AddMethod(methods.AddDecoder);
    inlMod->AddMethod(methods.Listen);
    inlMod->AddMethod(methods.Connect);
    inlMod->AddMethod(methods.AsyncConn);
    inlMod->AddMethod(methods.RemoveSession);
    inlMod->AddMethod(methods.IsSessionValidate);
    inlMod->AddMethod(methods.SendData);
    inlMod->AddMethod(methods.Multicast);
    inlMod->AddMethod(methods.Broadcast);
    inlMod->AddMethod(methods.Subscribe);
    inlMod->AddMethod(methods.PreSubscribe);
    inlMod->AddMethod(methods.UnifyPreSubscribe);
    inlMod->AddMethod(methods.SubscribeEvent);
    inlMod->AddMethod(methods.UnsubscribeEventById);
    inlMod->AddMethod(methods.UnsubscribeEventByStub);
    inlMod->AddMethod(methods.FireEvent);
    inlMod->AddMethod(methods.InstallErrHooker);
    inlMod->AddMethod(methods.UninstallErrHooker);
    inlMod->AddMethod(methods.ClearHookedErrors);
    inlMod->AddMethod(methods.Post);
    inlMod->AddMethod(methods.CallComponentMethod);
    inlMod->AddMethod(methods.ServiceMainLoop);

    // Add Obj<-->Json coder helper to top module.
    topMod->AddMethod(methods.j2o);
    topMod->AddMethod(methods.o2j);

    // Create protocol sub module.
    pyllbc_Module *protoMod = new pyllbc_Module("Protocol", topMod);
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
