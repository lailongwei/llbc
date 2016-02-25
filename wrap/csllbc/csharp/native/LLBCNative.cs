/**
 * @file    NativeLLBC.cs
* @author   Longwei Lai<lailongwei@126.com>
* @date     2016/01/25
* @version  1.0
*
* @brief
*/

using System.Runtime.InteropServices;
namespace llbc
{
    /// <summary>
    /// The all llbc native methods declare.
    /// </summary>
    internal partial class LLBCNative
    {
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
#else
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
