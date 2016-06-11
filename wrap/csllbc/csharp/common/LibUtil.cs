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
        /// <returns>The native string pointer(alloc from unmanaged memory area)</returns>
        public static IntPtr CreateNativeStr(string str, IntPtr nativeLen)
        {
            if (string.IsNullOrEmpty(str))
            {
                unsafe
                {
                    *(int*)nativeLen.ToPointer() = 0;
                }
                return IntPtr.Zero;
            }

            byte[] bytes = Encoding.UTF8.GetBytes(str);
            IntPtr unmanaged = Marshal.AllocHGlobal(bytes.Length);
            Marshal.Copy(bytes, 0, unmanaged, bytes.Length);

            unsafe
            {
                *(int*)nativeLen.ToPointer() = bytes.Length;
            }

            return unmanaged;
        }

        /// <summary>
        /// Create native string.
        /// </summary>
        /// <param name="str">the managed string object</param>
        /// <param name="nativeLen">native string length</param>
        /// <returns>the native string pointer(alloc from unmanaged memory area)</returns>
        public static IntPtr CreateNativeStr(string str, out int nativeLen)
        {
            unsafe
            {
                fixed (int* ptr = &nativeLen)
                {
                    return CreateNativeStr(str, new IntPtr(ptr));
                }
            }
        }
    }
}