// The MIT License (MIT)

// Copyright (c) 2013-2024 lailongwei<lailongwei@126.com>
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

//!!! This file is auto generated by script tool, do not modify it!!!

using System;
using System.Runtime.InteropServices;

namespace llbc
{

    /// <summary>
    /// Wrap common/_LibConfig.h file defined c functions
    /// </summary>
    internal partial class LLBCNative
    {
        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_DefaultBacklogSize();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_RootLoggerName(long name, int nameLen);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultNotConfigOptionUse(long notConfigUse, int len);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultLogLevel();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_IsDirectFlushToConsole();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultIsAsyncMode();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultLogToConsole();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultConsoleLogPattern(long pattern, int patternLen);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_IsDefaultColourfulOutput();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_IsDefaultLogToFile();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultFileLogPattern(long pattern, int patternLen);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultFileRollingMode();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static long csllbc_LibConfig_Log_MaxFileSizeLimit();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static long csllbc_LibConfig_Log_DefaultMaxFileSize();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_MaxBackupIndex();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultLogFileBufferSize();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_DefaultLogFlushInterval();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_MaxLogFlushInterval();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_TakeOverUnknownLoggerMsg();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Log_LazyCreateLogFile();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Thread_GuardDebug();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Timer_StrictSchedule();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_IsNetOrder();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_DefaultConnTimeout();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_MaxEventCount();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_DefaultServiceFPS();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_MinServiceFPS();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_MaxServiceFPS();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_PerThreadDriveMaxServiceCount();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_MaxCompNameLen();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_IsEnabledStatusHandler();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_IsEnabledUnifyPreSubscribe();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_LibConfig_Comm_PollerModel(long model, int modelLen);
    }
}
