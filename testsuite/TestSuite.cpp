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
    test = new TestCase_Com_Version;
    // test = new TestCase_Com_DataType;
    // test = new TestCase_Com_Endian;
    // test = new TestCase_Com_Stream;
    // test = new TestCase_Com_Error;
    // test = new TestCase_Com_Compiler;
    // test = new TestCase_Com_RTTI;

    /* Core module testcases. */
    // test = new TestCase_Core_OS_Thread;
    // test = new TestCase_Core_OS_Console;
    // test = new TestCase_Core_Bundle;
    // test = new TestCase_Core_Utils_Text;
    // test = new TestCase_Core_Utils_Debug;
    // test = new TestCase_Core_Utils_Algorithm;
    // test = new TestCase_Core_Utils_Delegate;
    // test = new TestCase_Core_Utils_MD5;
    // test = new TestCase_Core_Utils_Misc;
    // test = new TestCase_Core_Utils_Network;
    // test = new TestCase_Core_Helper_JVHelper;
    // test = new TestCase_Core_Helper_StlHelper;
    // test = new TestCase_Core_File_File;
    // test = new TestCase_Core_File_Directory;
    // test = new TestCase_Core_VariantTest;
    // test = new TestCase_Core_Config_Ini;
    // test = new TestCase_Core_Config_Config;
    // test = new TestCase_Core_Time_Time;
    // test = new TestCase_Core_Config_Property;
    // test = new TestCase_Core_Thread_Lock;
    // test = new TestCase_Core_Thread_CV;
    // test = new TestCase_Core_Thread_RWLock;
    // test = new TestCase_Core_Thread_Semaphore;
    // test = new TestCase_Core_Thread_Tls;
    // test = new TestCase_Core_Thread_ThreadMgr;
    // test = new TestCase_Core_Thread_Task;
    // test = new TestCase_Core_Random;
    // test = new TestCase_Core_Log;
    // test = new TestCase_Core_Entity;
    // test = new TestCase_Core_Transcoder;
    // test = new TestCase_Core_Library;

    /* ObjBase module testcases. */
#if LLBC_CFG_OBJBASE_ENABLED
    // test = new TestCase_ObjBase_Object;
    // test = new TestCase_ObjBase_Array;
    // test = new TestCase_ObjBase_Dictionary;
#endif // LLBC_CFG_OBJBASE_ENABLED

    /* Communication module testcases. */
    // test = new TestCase_Comm_Event;
    // test = new TestCase_Comm_Timer;
    // test = new TestCase_Comm_PacketOp;
#if LLBC_CFG_OBJBASE_ENABLED
    // test = new TestCase_Comm_ReleasePool;
#endif // LLBC_CFG_OBJBASE_ENABLED
    // test = new TestCase_Comm_Facade;
    // test = new TestCase_Comm_SvcBase;
    // test = new TestCase_Comm_SvcFps;
    // test = new TestCase_Comm_SvcStartStop;
    // test = new TestCase_Comm_Svc;
    // test = new TestCase_Comm_SendBytes;
    // test = new TestCase_Comm_Multicast;
    // test = new TestCase_Comm_ExternalDriveSvc;
    // test = new TestCase_Comm_LazyTask;

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
