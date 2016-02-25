/**
 * @file    Errors.cs
 * @author  Longwei Lai<lailngwei@126.com>
 * @date    2016/01/25
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
    /// Exception util class encapsulation.
    /// </summary>
    internal class ExceptionUtil
    {
        /// <summary>
        /// Create exception from llbc core library.
        /// </summary>
        /// <returns></returns>
        public static LLBCException CreateExceptionFromCoreLib()
        {
            unsafe
            {
                int errStrLen = 0;
                IntPtr errStr = LLBCNative.csllbc_FormatLastError(new IntPtr(&errStrLen));

                if (errStrLen > 0)
                {
                    byte[] bytes = new byte[errStrLen];
                    Marshal.Copy(errStr, bytes, 0, errStrLen);
                    return new LLBCException(Encoding.UTF8.GetString(bytes));
                }
                else
                {
                    return new LLBCException("unknown error");
                }
            }
        }
    }
}
