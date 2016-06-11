/**
 * @file    StringUtil.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/26
 * @version 1.0
 *
 * @brief
 */

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

            int uniqueueId = 0;
            byte[] bytes = System.Text.Encoding.UTF8.GetBytes(str);
            for (int i = 0; i < bytes.Length; i++)
                uniqueueId += i * 0x31 * bytes[i];

            uniqueueId += bytes.Length * bytes[0];
            uniqueueId *= bytes[bytes.Length - 1];

            return uniqueueId;
        }
    }
}