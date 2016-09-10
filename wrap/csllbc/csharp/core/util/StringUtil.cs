/**
 * @file    StringUtil.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/26
 * @version 1.0
 *
 * @brief
 */

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