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

namespace llbc
{
    /// <summary>
    /// Log class encapsulation, use for simply to access root Logger.
    /// </summary>
    public class Log
    {
        #region properties: enabled, enabledLogFileInfo
        /// <summary>
        /// Check log enabled or not.
        /// </summary>
        public static bool enabled
        {
            get { return LoggerMgr.GetRoot().enabled; }
            set { LoggerMgr.GetRoot().enabled = value; }
        }

        /// <summary>
        /// Enabled log file info or not.
        /// </summary>
        public static bool enabledLogFileInfo
        {
            get { return LoggerMgr.GetRoot().enabledLogFileInfo; }
            set { LoggerMgr.GetRoot().enabledLogFileInfo = value; }
        }
        #endregion

        #region Dbg/Info/Warn/Err/Fatal
        /// <summary>
        /// Log Debug level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Dbg(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Dbg(1, fmt, args);
        }

        /// <summary>
        /// Log Debug level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Dbg(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Dbg(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Info(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Info(1, fmt, args);
        }

        /// <summary>
        /// Log Info level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Info(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Info(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Warn(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Warn(1, fmt, args);
        }

        /// <summary>
        /// Log Warn level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Warn(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Warn(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Err(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Err(1, fmt, args);
        }

        /// <summary>
        /// Log Error level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Err(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Err(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Fatal(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Fatal(1, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Fatal(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Fatal(1 + skipFrames, fmt, args);
        }
        #endregion // Dbg/Info/Warn/Err/Fatal

        #region Dbg<Tag>/Info<Tag>/Warn<Tag>/Err<Tag>/Fatal<Tag>
        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Dbg<Tag>(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Dbg<Tag>(1, fmt, args);
        }

        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Dbg<Tag>(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Dbg<Tag>(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Info<Tag>(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Info<Tag>(1, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Info<Tag>(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Info<Tag>(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Warn<Tag>(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Warn<Tag>(1, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Warn<Tag>(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Warn<Tag>(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Err<Tag>(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Err<Tag>(1, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Err<Tag>(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Err<Tag>(1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Fatal<Tag>(string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Fatal<Tag>(1, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Fatal<Tag>(int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Fatal<Tag>(1 + skipFrames, fmt, args);
        }
        #endregion // Dbg<Tag>/Info<Tag>/Warn<Tag>/Err<Tag>/Fatal<Tag>

        #region Dbg2/Info2/Warn2/Err2/Fatal2
        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Dbg2(object tag, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Dbg2(tag, 1, fmt, args);
        }

        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Dbg2(object tag, int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Dbg2(tag, 1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Info2(object tag, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Info2(tag, 1, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Info2(object tag, int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Info2(tag, 1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Warn2(object tag, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Warn2(tag, 1, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Warn2(object tag, int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Warn2(tag, 1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Err2(object tag, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Err2(tag, 1, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Err2(object tag, int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Err2(tag, 1 + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Fatal2(object tag, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Fatal2(tag, 1, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="tag">log message tag</param>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public static void Fatal2(object tag, int skipFrames, string fmt, params object[] args)
        {
            LoggerMgr.GetRoot().Fatal2(tag, 1 + skipFrames, fmt, args);
        }
        #endregion // Dbg2/Info2/Warn2/Err2/Fatal2
    }
}