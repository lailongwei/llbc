/**
 * @file    Exception.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */

using System;

namespace llbc
{
    /// <summary>
    /// The llbc library exception class encapsulation.
    /// </summary>
    public class LLBCException : Exception
    {
        public LLBCException()
        {
        }

        public LLBCException(string message)
        : base(message)
        {
        }

        public LLBCException(string messageFmt, params object[] args)
        : base(string.Format(messageFmt, args))
        {
        }
    }
}
