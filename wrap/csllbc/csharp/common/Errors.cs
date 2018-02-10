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
