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
        /// <param name="ptr">pointer</param>
        /// <param name="len">pointer length</param>
        /// <returns>the converted string</returns>
        /// </summary>
        public static unsafe string Ptr2Str(IntPtr ptr, int len)
        {
            return new string((sbyte *)ptr.ToPointer(), 0, len, Encoding.UTF8);
        }

        /// <summary>
        /// Convert pointer to string.
        /// </summary>
        /// <param name="ptr">pointer</param>
        /// <param name="len">pointer length</param>
        /// <returns>the converted string</returns>
        public static unsafe string Ptr2Str(byte* ptr, int len)
        {
            return new string((sbyte *)ptr, 0, len, Encoding.UTF8);
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

        /// <summary>
        /// Free native pointer resource.
        /// </summary>
        /// <param name="ptr">the native resource pointer</param>
        public unsafe static void FreeNativePtr(void* ptr)
        {
            var managedPtr = new IntPtr(ptr);
            if (ptr != (void*)0)
                FreeNativePtr(ref managedPtr, false);
        }

        /// <summary>
        /// Free native pointer resource.
        /// </summary>
        /// <param name="ptr">the native resource pointer</param>
        /// <param name="setToZero">after free native pointer resource, need set to Zero or not</param>
        public static void FreeNativePtr(ref IntPtr ptr, bool setToZero = true)
        {
            if (ptr != IntPtr.Zero)
            {
                System.Runtime.InteropServices.Marshal.FreeHGlobal(ptr);
                if (setToZero)
                    ptr = IntPtr.Zero;
            }
        }
    }
}