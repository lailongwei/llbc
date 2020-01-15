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


#include "TestSuite.h"

int TestSuite_Main(int argc, char *argv[])
{
    ::llbc::LLBC_Startup();

    ::llbc::LLBC_ITestCase *test = NULL;

    /* Common module testcases. */
    test = LLBC_New0(TestCase_Com_Version);
    // test = LLBC_New0(TestCase_Com_DataType);
    // test = LLBC_New0(TestCase_Com_Endian);
    // test = LLBC_New0(TestCase_Com_Stream);
    // test = LLBC_New0(TestCase_Com_Error);
    // test = LLBC_New0(TestCase_Com_Compiler);
    // test = LLBC_New0(TestCase_Com_RTTI);

    /* Core module testcases. */
    // test = LLBC_New0(TestCase_Core_OS_Symbol);
    // test = LLBC_New0(TestCase_Core_OS_Thread);
    // test = LLBC_New0(TestCase_Core_OS_Console);
    // test = LLBC_New0(TestCase_Core_Algo_RingBuffer);
    // test = LLBC_New0(TestCase_Core_Bundle);
    // test = LLBC_New0(TestCase_Core_Utils_Text);
    // test = LLBC_New0(TestCase_Core_Utils_Debug);
    // test = LLBC_New0(TestCase_Core_Utils_Algorithm);
    // test = LLBC_New0(TestCase_Core_Utils_Delegate);
    // test = LLBC_New0(TestCase_Core_Utils_MD5);
    // test = LLBC_New0(TestCase_Core_Utils_Base64);
    // test = LLBC_New0(TestCase_Core_Utils_Misc);
    // test = LLBC_New0(TestCase_Core_Utils_Network);
    // test = LLBC_New0(TestCase_Core_Helper_StlHelper);
    // test = LLBC_New0(TestCase_Core_File_File);
    // test = LLBC_New0(TestCase_Core_File_Directory);
    // test = LLBC_New0(TestCase_Core_VariantTest);
    // test = LLBC_New0(TestCase_Core_Config_Ini);
    // test = LLBC_New0(TestCase_Core_Time_Time);
    // test = LLBC_New0(TestCase_Core_Event);
    // test = LLBC_New0(TestCase_Core_Config_Property);
    // test = LLBC_New0(TestCase_Core_Thread_Lock);
    // test = LLBC_New0(TestCase_Core_Thread_RWLock);
    // test = LLBC_New0(TestCase_Core_Thread_Guard);
    // test = LLBC_New0(TestCase_Core_Thread_CV);
    // test = LLBC_New0(TestCase_Core_Thread_Semaphore);
    // test = LLBC_New0(TestCase_Core_Thread_Tls);
    // test = LLBC_New0(TestCase_Core_Thread_ThreadMgr);
    // test = LLBC_New0(TestCase_Core_Thread_Task);
    // test = LLBC_New0(TestCase_Core_Random);
    // test = LLBC_New0(TestCase_Core_Log);
    // test = LLBC_New0(TestCase_Core_Entity);
    // test = LLBC_New0(TestCase_Core_Transcoder);
    // test = LLBC_New0(TestCase_Core_Library);
    // test = LLBC_New0(TestCase_ObjBase_Object);
    // test = LLBC_New0(TestCase_ObjBase_Array);
    // test = LLBC_New0(TestCase_ObjBase_Dictionary);
    // test = LLBC_New0(TestCase_Core_ObjectPool);
    // test = LLBC_New0(TestCase_Core_Recycle);

    /* Communication module testcases. */
    // test = LLBC_New0(TestCase_Comm_EventInSvc);
    // test = LLBC_New0(TestCase_Comm_Timer);
    // test = LLBC_New0(TestCase_Comm_PacketOp);
    // test = LLBC_New0(TestCase_Comm_ReleasePool);
    // test = LLBC_New0(TestCase_Comm_Facade);
    // test = LLBC_New0(TestCase_Comm_SvcBase);
    // test = LLBC_New0(TestCase_Comm_SvcFps);
    // test = LLBC_New0(TestCase_Comm_SvcStartStop);
    // test = LLBC_New0(TestCase_Comm_Svc);
    // test = LLBC_New0(TestCase_Comm_SendBytes);
    // test = LLBC_New0(TestCase_Comm_Multicast);
    // test = LLBC_New0(TestCase_Comm_ExternalDriveSvc);
    // test = LLBC_New0(TestCase_Comm_LazyTask);
    // test = LLBC_New0(TestCase_Comm_ProtoStackCtrl);

    // test = LLBC_New0(TestCase_App_AppTest);

    int ret = LLBC_FAILED;
    if (test)
    {
        ret = test->Run(argc, argv);
        LLBC_Delete(test);
    }
    else
    {
        LLBC_PrintLine("Not specific any testcase to run!");
    }

    ::llbc::LLBC_Cleanup();

    return ret;
}
