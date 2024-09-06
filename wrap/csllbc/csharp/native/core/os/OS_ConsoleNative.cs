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
    /// Wrap core/os/_OS_Console.h file defined c functions
    /// </summary>
    internal partial class LLBCNative
    {
        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static void csllbc_Console_Trace(IntPtr value, int valueLen);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static void csllbc_Console_SafePrint(bool toStdout, bool newLine, IntPtr value, int valueLen);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Console_GetForegroundColor();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Console_SetForegroundColor(int color);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Console_GetBackgroundColor();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Console_SetBackgroundColor(int color);

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Console_ResetColor();

        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Console_SafeFlush(bool flushStdout);
    }
}
