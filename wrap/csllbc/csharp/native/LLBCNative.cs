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

using System.Runtime.InteropServices;
namespace llbc
{
    /// <summary>
    /// The all llbc native methods declare.
    /// </summary>
    internal partial class LLBCNative
    {
        #region Startup/Cleanup llbc library
        /// <summary>
        /// Startup the llbc library.
        /// </summary>
        /// <returns></returns>
        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Startup();

        /// <summary>
        /// Cleanup the llbc library.
        /// </summary>
        /// <returns></returns>
        [DllImport(NativeLibName, CallingConvention = CallingConvention.Cdecl)]
        public extern static int csllbc_Cleanup();
        #endregion

        # region Library some constants
        /// <summary>
        /// All llbc native library functions execute success return code.
        /// </summary>
        public const int LLBC_OK = 0;

        /// <summary>
        /// All llbc native library functions execute failed return code.
        /// </summary>
        public const int LLBC_FAILED = -1;
        #endregion

        #region llbc core library name & native library name define
#if !DEBUG
        /// <summary>
        /// The llbc core library name define.
        /// </summary>
        public const string CoreLibName = "libllbc.dll";

        /// <summary>
        /// The llbc c# native library name define.
        /// </summary>
        public const string NativeLibName = "libcsllbc_native.dll";
#else // Debug
        /// <summary>
        /// The llbc core library name define.
        /// </summary>
        public const string CoreLibName = "libllbc_debug.dll";

        /// <summary>
        /// The llbc c# native library name define.
        /// </summary>
        public const string NativeLibName = "libcsllbc_native_debug.dll";
#endif
        #endregion
    }
}
