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
        public static LLBCException CreateExceptionFromCoreLib(uint errNo = 0)
        {
            unsafe
            {
                int errStrLen = 0;
                IntPtr errStr;

                if (errNo == 0)
                    errStr = LLBCNative.csllbc_FormatLastError(new IntPtr(&errStrLen));
                else
                    errStr = LLBCNative.csllbc_StrError(errNo, new IntPtr(&errStrLen));

                if (errStrLen > 0)
                    return new LLBCException(LibUtil.Ptr2Str(errStr, errStrLen));
                
                return new LLBCException("unknown error");
            }
        }
    }
}
