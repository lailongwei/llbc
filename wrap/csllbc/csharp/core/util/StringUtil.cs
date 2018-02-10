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
using System.Text;

namespace llbc
{
    /// <summary>
    /// String utility class encapsulation.
    /// </summary>
    public class StringUtil
    {
        /// <summary>
        /// Get string unique Id.
        /// </summary>
        /// <param name="str">string</param>
        /// <returns>the unique Id</returns>
        public static int UniqueId(string str)
        {
            if (string.IsNullOrEmpty(str))
                return 0;

            byte[] bytes = Encoding.UTF8.GetBytes(str);
            unsafe
            {
                fixed (byte* ptr = &bytes[0])
                {
                    return LLBCNative.csllbc_Utils_String_HashString(new IntPtr(ptr), bytes.Length);
                }
            }
        }

        /// <summary>
        /// Convert string value to specific type value.
        /// </summary>
        /// <typeparam name="T">will convert to type</typeparam>
        /// <param name="str">string value</param>
        /// <param name="dftValue">if convert failed, will return the default value</param>
        /// <returns>converted value</returns>
        public static T ToType<T>(string str, T dftValue = default(T))
        {
            if (string.IsNullOrEmpty(str) && typeof(T) != typeof(string))
                return dftValue;
            try
            {
                if (typeof(T) == typeof(bool))
                {
                    string lowercasedStr = str.ToLower();
                    if (lowercasedStr == "true")
                        return (T)((object)true);
                    else if (lowercasedStr == "false")
                        return (T)((object)false);
                }

                return (T)Convert.ChangeType(str, typeof(T));
            }
            catch (Exception)
            {
                return dftValue;
            }
        }
    }
}