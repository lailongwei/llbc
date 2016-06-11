/**
 * @file    TestSuite.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_SUITE_H__
#define __LLBC_TEST_SUITE_H__

#include "common/TestCase_Com_DataType.h"
#include "common/TestCase_Com_Endian.h"
#include "common/TestCase_Com_Stream.h"
#include "common/TestCase_Com_Error.h"
#include "common/TestCase_Com_Version.h"
#include "common/TestCase_Com_Compiler.h"

#include "core/os/TestCase_Core_OS_Thread.h"
#include "core/os/TestCase_Core_OS_Console.h"
#include "core/bundle/TestCase_Core_Bundle.h"
#include "core/utils/TestCase_Core_Utils_Text.h"
#include "core/utils/TestCase_Core_Utils_Debug.h"
#include "core/utils/TestCase_Core_Utils_Algorithm.h"
#include "core/utils/TestCase_Core_Utils_Delegate.h"
#include "core/utils/TestCase_Core_Utils_MD5.h"
#include "core/utils/TestCase_Core_Utils_Misc.h"
#include "core/utils/TestCase_Core_Utils_Network.h"
#include "core/helper/TestCase_Core_Helper_JVHelper.h"
#include "core/helper/TestCase_Core_Helper_StlHelper.h"
#include "core/file/TestCase_Core_File_File.h"
#include "core/file/TestCase_Core_File_Directory.h"
#include "core/variant/TestCase_Core_VariantTest.h"
#include "core/config/TestCase_Core_Config_Config.h"
#include "core/config/TestCase_Core_Config_Property.h"
#include "core/time/TestCase_Core_Time_Time.h"
#include "core/thread/TestCase_Core_Thread_Lock.h"
#include "core/thread/TestCase_Core_Thread_RWLock.h"
#include "core/thread/TestCase_Core_Thread_CV.h"
#include "core/thread/TestCase_Core_Thread_Semaphore.h"
#include "core/thread/TestCase_Core_Thread_Tls.h"
#include "core/thread/TestCase_Core_Thread_ThreadMgr.h"
#include "core/thread/TestCase_Core_Thread_Task.h"
#include "core/random/TestCase_Core_Random.h"
#include "core/log/TestCase_Core_Log.h"
#include "core/entity/TestCase_Core_Entity.h"
#include "core/transcoder/TestCase_Core_Transcoder.h"
#include "core/library/TestCase_Core_Library.h"

#include "objbase/TestCase_ObjBase_Object.h"
#include "objbase/TestCase_ObjBase_Array.h"
#include "objbase/TestCase_ObjBase_Dictionary.h"

#include "comm/TestCase_Comm_Event.h"
#include "comm/TestCase_Comm_Timer.h"
#include "comm/TestCase_Comm_PacketOp.h"
#include "comm/TestCase_Comm_HeaderDesc.h"
#include "comm/TestCase_Comm_PacketHeaderParts.h"
#include "comm/TestCase_Comm_ReleasePool.h"
#include "comm/TestCase_Comm_Facade.h"
#include "comm/TestCase_Comm_SvcBase.h"
#include "comm/TestCase_Comm_Svc.h"
#include "comm/TestCase_Comm_SendBytes.h"
#include "comm/TestCase_Comm_Multicast.h"
#include "comm/TestCase_Comm_ExternalDriveSvc.h"
#include "comm/TestCase_Comm_LazyTask.h"
#include "comm/TestCase_Comm_CustomHeaderSvc.h"

extern int TestSuite_Main(int argc, char *argv[]);

#endif // !__LLBC_TEST_SUITE_H__
