/**
 * @file    LibUtil.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/21
 * @version 1.0
 *
 * @brief
 */

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace llbc
{
    /// <summary>
    /// llbc library internal utility class encapsulation.
    /// </summary>
    internal partial class LibUtil
    {
        /// <summary>
        /// Convert Pointer to string.
        /// </summary>
        public static unsafe string Ptr2Str(IntPtr ptr, int len)
        {
            return new string((sbyte *)ptr.ToPointer(), 0, len, Encoding.UTF8);
        }

        /// <summary>
        /// Create native string.
        /// </summary>
        /// <param name="str">the managed string object</param>
        /// <param name="nativeLen">native string length</param>
        /// <param name="appendNull">auto append \0 character option, default is true</param>
        /// <returns>The native string pointer(alloc from unmanaged memory area)</returns>
        public static IntPtr CreateNativeStr(string str, IntPtr nativeLen, bool appendNull = true)
        {
            if (string.IsNullOrEmpty(str))
            {
                unsafe
                {
                    *(int*)nativeLen.ToPointer() = 0;
                }
                return IntPtr.Zero;
            }


            int strBytesLen = Encoding.UTF8.GetByteCount(str);
            byte[] bytes = new byte[strBytesLen + (appendNull ? 1 : 0)];
            Encoding.UTF8.GetBytes(str, 0, str.Length, bytes, 0);

            IntPtr unmanaged = Marshal.AllocHGlobal(bytes.Length);
            Marshal.Copy(bytes, 0, unmanaged, bytes.Length);
            unsafe
            {
                *(int*)nativeLen.ToPointer() = strBytesLen;
            }

            return unmanaged;
        }

        /// <summary>
        /// Create native string.
        /// </summary>
        /// <param name="str">the managed string object</param>
        /// <param name="nativeLen">native string length</param>
        /// <param name="appendNull">auto append \0 character option, default is true</param>
        /// <returns>the native string pointer(alloc from unmanaged memory area)</returns>
        public static IntPtr CreateNativeStr(string str, out int nativeLen, bool appendNull = true)
        {
            unsafe
            {
                fixed (int* ptr = &nativeLen)
                {
                    return CreateNativeStr(str, new IntPtr(ptr), appendNull);
                }
            }
        }

        /// <summary>
        /// Create native string.
        /// </summary>
        /// <param name="str">the managed string object</param>
        /// <param name="appendNull">auto append \0 character option, default is true</param>
        /// <returns></returns>
        public static IntPtr CreateNativeStr(string str, bool appendNull = true)
        {
            int len;
            return CreateNativeStr(str, out len, appendNull);
        }
    }
}