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
    /// The safe console class simple encapsulation.
    /// </summary>
    public static class SafeConsole
    {
        #region foregroundColor/backgroundColor, ResetColor
        /// <summary>
        /// Console foreground color getter/setter.
        /// </summary>
        public static ConsoleColor foregroundColor
        {
            get
            { 
                int color = LLBCNative.csllbc_Console_GetForegroundColor();
                if (color == LLBCNative.LLBC_FAILED)
                    return ConsoleColor.White;

                return (ConsoleColor)color;
            }

            set
            {
                LLBCNative.csllbc_Console_SetForegroundColor((int)value);
            }
        }

        /// <summary>
        /// Console background color getter/setter.
        /// </summary>
        public static ConsoleColor backgroundColor
        {
            get
            {
                int color = LLBCNative.csllbc_Console_GetBackgroundColor();
                if (color == LLBCNative.LLBC_FAILED)
                    return ConsoleColor.Black;

                return (ConsoleColor)color;
            }

            set
            {
                LLBCNative.csllbc_Console_SetBackgroundColor((int)value);
            }
        }

        /// <summary>
        /// Reset console color.
        /// </summary>
        public static void ResetColor()
        {
            LLBCNative.csllbc_Console_ResetColor();
        }
        #endregion

        #region Read/ReadKey
        /// <summary>
        /// Read next character from input stream.
        /// <para>wrapped from Console.Read() method</para>
        /// </summary>
        /// <returns>character</returns>
        public static int Read()
        {
            return Console.Read();
        }

        /// <summary>
        /// Get user press key info.
        /// <para>wrapped from Console.ReadKey() method</para>
        /// </summary>
        /// <returns></returns>
        public static ConsoleKeyInfo ReadKey()
        {
            return Console.ReadKey();
        }

        /// <summary>
        /// Read user press key info.
        /// </summary>
        /// <param name="intercept">Determines whether to display the pressed key in the console window. true to not display the pressed key; otherwise, false</param>
        /// <returns></returns>
        public static ConsoleKeyInfo ReadKey(bool intercept)
        {
            return Console.ReadKey(intercept);
        }
        #endregion

        #region Write
        /// <summary>
        /// Write bool value to standard output console.
        /// </summary>
        /// <param name="value">bool value</param>
        public static void Write(bool value)
        {
            _Write(true, false, value);
        }

        /// <summary>
        /// Write char value to standard output console.
        /// </summary>
        /// <param name="value">char value</param>
        public static void Write(char value)
        {
            _Write(true, false, value);
        }

        /// <summary>
        /// Write object value to standard output console.
        /// </summary>
        /// <param name="value">object value</param>
        public static void Write(object value)
        {
            _Write(true, false, value);
        }

        /// <summary>
        /// Format output specific datas to standard output console.
        /// </summary>
        /// <param name="fmt">format string</param>
        /// <param name="args">will format parameters</param>
        public static void Write(string fmt, params object[] args)
        {
            _Write(true, false, fmt, args);
        }
        #endregion

        #region WriteLine
        /// <summary>
        /// Write bool value to standard output console and append LF character.
        /// </summary>
        /// <param name="value">bool value</param>
        public static void WriteLine(bool value)
        {
            _Write(true, true, value);
        }

        /// <summary>
        /// Write char value to standard output console and append LF character.
        /// </summary>
        /// <param name="value">char value</param>
        public static void WriteLine(char value)
        {
            _Write(true, true, value);
        }

        /// <summary>
        /// Write object value to standard output console and append LF character.
        /// </summary>
        /// <param name="value">object value</param>
        public static void WriteLine(object value)
        {
            _Write(true, true, value);
        }

        /// <summary>
        /// Format output specific datas to standard output console and append LF character.
        /// </summary>
        /// <param name="fmt">format string</param>
        /// <param name="args">will format parameters</param>
        public static void WriteLine(string fmt, params object[] args)
        {
            _Write(true, true, fmt, args);
        }
        #endregion

        #region WriteError
        /// <summary>
        /// Write bool value to standard error console.
        /// </summary>
        /// <param name="value">bool value</param>
        public static void WriteError(bool value)
        {
            _Write(false, false, value);
        }

        /// <summary>
        /// Write char value to standard error console.
        /// </summary>
        /// <param name="value">char value</param>
        public static void WriteError(char value)
        {
            _Write(false, false, value);
        }

        /// <summary>
        /// Write object value to standard error console.
        /// </summary>
        /// <param name="value">object value</param>
        public static void WriteError(object value)
        {
            _Write(false, false, value);
        }

        /// <summary>
        /// Format output specific datas to standard error console.
        /// </summary>
        /// <param name="fmt">format string</param>
        /// <param name="args">will format parameters</param>
        public static void WriteError(string fmt, params object[] args)
        {
            _Write(false, false, fmt, args);
        }
        #endregion

        #region WriteErrorLine
        /// <summary>
        /// Write bool value to standard error console and append LF character.
        /// </summary>
        /// <param name="value">bool value</param>
        public static void WriteErrorLine(bool value)
        {
            _Write(false, true, value);
        }

        /// <summary>
        /// Write char value to standard error console and append LF character.
        /// </summary>
        /// <param name="value">char value</param>
        public static void WriteErrorLine(char value)
        {
            _Write(false, true, value);
        }

        /// <summary>
        /// Write object value to standard error console and append LF character.
        /// </summary>
        /// <param name="value">object value</param>
        public static void WriteErrorLine(object value)
        {
            _Write(false, true, value);
        }

        /// <summary>
        /// Format output specific datas to standard error console and append LF character.
        /// </summary>
        /// <param name="fmt">format string</param>
        /// <param name="args">will format parameters</param>
        public static void WriteErrorLine(string fmt, params object[] args)
        {
            _Write(false, true, fmt, args);
        }
        #endregion

        #region Flush
        /// <summary>
        /// Flush all standard output buffer to console.
        /// </summary>
        public static void Flush()
        {
            LLBCNative.csllbc_Console_SafeFlush(true);
        }

        /// <summary>
        /// Flush all standard error buffer to console.
        /// </summary>
        public static void FlushError()
        {
            LLBCNative.csllbc_Console_SafeFlush(false);
        }
        #endregion

        #region Trace
        /// <summary>
        /// Trace message, only output when enabled LLBC_DEBUG macro in llbc core library.
        /// </summary>
        /// <param name="msg">will trace message</param>
        public static void Trace(string msg)
        {
            if (msg.Length == 0)
                return;

            byte[] msgBytes = Encoding.UTF8.GetBytes(msg);
            unsafe
            {
                fixed (byte* ptr = &msgBytes[0])
                {
                    LLBCNative.csllbc_Console_Trace(new IntPtr(ptr), msgBytes.Length);
                }
            }
        }

        /// <summary>
        /// Trace message, only output when enabled LLBC_DEBUG macro in llbc core library.
        /// </summary>
        /// <param name="fmt">message formatter</param>
        /// <param name="args">format arguments</param>
        public static void Trace(string fmt, params object[] args)
        {
            Trace(string.Format(fmt, args));
        }
        #endregion

        #region Internal implementations
        private static void _Write(bool toStdout, bool newLine, bool value)
        {
            _SafeOutput(toStdout, newLine, value ? "true" : "false");
        }

        private static void _Write(bool toStdout, bool newLine, char value)
        {
            unsafe
            {
                byte* bytes = stackalloc byte[32];
                int wrote = Encoding.UTF8.GetBytes(&value, 1, bytes, 32);
                LLBCNative.csllbc_Console_SafePrint(toStdout, newLine, new IntPtr(bytes), wrote);
            }
        }

        private static void _Write(bool toStdout, bool newLine, object value)
        {
            _SafeOutput(toStdout, newLine, value.ToString());
        }

        public static void _Write(bool toStdout, bool newLine, string fmt, params object[] args)
        {
            _SafeOutput(toStdout, newLine, string.Format(fmt, args));
        }

        private static void _SafeOutput(bool toStdout, bool newLine, string content)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(content);
            if (bytes.Length == 0)
            {
                LLBCNative.csllbc_Console_SafePrint(toStdout, newLine, IntPtr.Zero, 0);
                return;
            }

            unsafe
            {
                fixed (byte *ptr = &bytes[0])
                {
                    LLBCNative.csllbc_Console_SafePrint(toStdout, newLine, new IntPtr(ptr), bytes.Length);
                }
            }
        }
        #endregion
    }
}