/**
 * @file    LibIniter.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/26
 * @version 1.0
 *
 * @brief
 */

using System;

namespace llbc
{
    /// <summary>
    /// llbc library initer, use for init/destroy llbc library.
    /// </summary>
    public class LibIniter
    {
        /// <summary>
        /// Init llbc library.
        /// </summary>
        public static void Init()
        {
            if (LLBCNative.csllbc_Startup() != 0)
                throw ExceptionUtil.CreateExceptionFromCoreLib();
        }

        /// <summary>
        /// Destroy llbc library.
        /// </summary>
        public static void Destroy()
        {
            if (LLBCNative.csllbc_Cleanup() != 0)
                throw ExceptionUtil.CreateExceptionFromCoreLib();
        }
    }
}