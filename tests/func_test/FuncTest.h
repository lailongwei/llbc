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

#include "common/FuncTest_Com_DataType.h"
#include "common/FuncTest_Com_Endian.h"
#include "common/FuncTest_Com_Stream.h"
#include "common/FuncTest_Com_StreamFunc.h"
#include "common/FuncTest_Com_Error.h"
#include "common/FuncTest_Com_Macro.h"
#include "common/FuncTest_Com_Version.h"
#include "common/FuncTest_Com_Compiler.h"
#include "common/FuncTest_Com_RTTI.h"
#include "common/FuncTest_Com_ConcatMacro.h"

#include "core/os/FuncTest_Core_OS_Symbol.h"
#include "core/os/FuncTest_Core_OS_Thread.h"
#include "core/os/FuncTest_Core_OS_Console.h"
#include "core/os/FuncTest_Core_OS_Process.h"
#include "core/os/FuncTest_Core_OS_Signal.h"
#include "core/algo/FuncTest_Core_Algo_Hash.h"
#include "core/algo/FuncTest_Core_Algo_RingBuffer.h"
#include "core/bundle/FuncTest_Core_Bundle.h"
#include "core/utils/FuncTest_Core_Utils_Text.h"
#include "core/utils/FuncTest_Core_Utils_Debug.h"
#include "core/utils/FuncTest_Core_Utils_Algorithm.h"
#include "core/utils/FuncTest_Core_Utils_Delegate.h"
#include "core/utils/FuncTest_Core_Utils_MD5.h"
#include "core/utils/FuncTest_Core_Utils_Base64.h"
#include "core/utils/FuncTest_Core_Utils_Misc.h"
#include "core/utils/FuncTest_Core_Utils_Network.h"
#include "core/helper/FuncTest_Core_Helper_StlHelper.h"
#include "core/file/FuncTest_Core_File_File.h"
#include "core/file/FuncTest_Core_File_Directory.h"
#include "core/variant/FuncTest_Core_Variant.h"
#include "core/config/FuncTest_Core_Config_Ini.h"
#include "core/config/FuncTest_Core_Config_Properties.h"
#include "core/time/FuncTest_Core_Time_Time.h"
#include "core/timer/FuncTest_Core_Timer_Heap.h"
#include "core/event/FuncTest_Core_Event.h"
#include "core/thread/FuncTest_Core_Thread_Lock.h"
#include "core/thread/FuncTest_Core_Thread_RWLock.h"
#include "core/thread/FuncTest_Core_Thread_Guard.h"
#include "core/thread/FuncTest_Core_Thread_CV.h"
#include "core/thread/FuncTest_Core_Thread_Semaphore.h"
#include "core/thread/FuncTest_Core_Thread_Tls.h"
#include "core/thread/FuncTest_Core_Thread_ThreadMgr.h"
#include "core/thread/FuncTest_Core_Thread_Task.h"
#include "core/random/FuncTest_Core_Random.h"
#include "core/log/FuncTest_Core_Log.h"
#include "core/entity/FuncTest_Core_Entity.h"
#include "core/transcoder/FuncTest_Core_Transcoder.h"
#include "core/library/FuncTest_Core_Library.h"
#include "core/objbase/FuncTest_Core_ObjBase_Object.h"
#include "core/objbase/FuncTest_Core_ObjBase_Array.h"
#include "core/objbase/FuncTest_Core_ObjBase_Dictionary.h"
#include "core/objpool/FuncTest_Core_ObjPool.h"
#include "comm/FuncTest_Comm_EventInSvc.h"
#include "comm/FuncTest_Comm_Timer.h"
#include "comm/FuncTest_Comm_PacketOp.h"
#include "comm/FuncTest_Comm_ReleasePool.h"
#include "comm/FuncTest_Comm_CompBase.h"
#include "comm/FuncTest_Comm_CompReflectionMethod.h"
#include "comm/FuncTest_Comm_CompEvent.h"
#include "comm/FuncTest_Comm_SvcBase.h"
#include "comm/FuncTest_Comm_SvcFps.h"
#include "comm/FuncTest_Comm_SvcStartStop.h"
#include "comm/FuncTest_Comm_Svc.h"
#include "comm/FuncTest_Comm_SendBytes.h"
#include "comm/FuncTest_Comm_Multicast.h"
#include "comm/FuncTest_Comm_ExternalDriveSvc.h"
#include "comm/FuncTest_Comm_LazyTask.h"
#include "comm/FuncTest_Comm_ProtoStackCtrl.h"
#include "comm/FuncTest_Comm_MessageBuffer.h"
#include "comm/FuncTest_Comm_DynLoadComp.h"
#include "comm/FuncTest_Comm_Echo.h"
#include "comm/FuncTest_Comm_RecentLoadInfo.h"

#include "app/FuncTest_App_AppTest.h"
#include "app/FuncTest_App_AppCfgTest.h"
#include "app/FuncTest_App_AppPhaseWaitingTest.h"
#include "app/FuncTest_App_AppTimer.h"
#include "app/FuncTest_App_ReloadFailedTest.h"

extern int FuncTest_Main(int argc, char *argv[]);


