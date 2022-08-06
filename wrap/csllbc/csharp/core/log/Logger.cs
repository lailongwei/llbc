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
    /// Log level enumeration.
    /// </summary>
    public enum LogLevel
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    }

    /// <summary>
    /// Logger class encapsulation.
    /// </summary>
    public class Logger
    {
        #region Ctor
        public Logger(string loggerName)
        {
            _loggerName = loggerName;
            IntPtr nativeLoggerName = LibUtil.CreateNativeStr(_loggerName);
            _nativeLogger = LLBCNative.csllbc_Log_GetLogger(nativeLoggerName);
            System.Runtime.InteropServices.Marshal.FreeHGlobal(nativeLoggerName);

            if (_nativeLogger == IntPtr.Zero)
                throw ExceptionUtil.CreateExceptionFromCoreLib();
        }
        #endregion // Ctor

        #region properties: loggerName, enabled, enabledLogFileInfo
        /// <summary>
        /// logger name.
        /// </summary>
        public string loggerName
        {
            get { return _loggerName; }
        }

        /// <summary>
        /// Check log enabled or not.
        /// </summary>
        public bool enabled
        {
            get { return _enabled; }
            set { _enabled = value; }
        }

        /// <summary>
        /// Enabled log file info or not.
        /// </summary>
        public bool enabledLogFileInfo
        {
            get { return _enabledLogFileInfo; }
            set { _enabledLogFileInfo = value; }
        }
        #endregion

        #region Trace/Dbg/Info/Warn/Err/Fatal
        /// <summary>
        /// Log Trace level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Trace(string fmt, params object[] args)
        {
            _Log(null, LogLevel.Trace, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Trace level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Trace(int skipFrames, string fmt, params object[] args)
        {
            _Log(null, LogLevel.Trace, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Debug level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Dbg(string fmt, params object[] args)
        {
            _Log(null, LogLevel.Debug, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Debug level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Dbg(int skipFrames, string fmt, params object[] args)
        {
            _Log(null, LogLevel.Debug, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Info(string fmt, params object[] args)
        {
            _Log(null, LogLevel.Info, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Info(int skipFrames, string fmt, params object[] args)
        {
            _Log(null, LogLevel.Info, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Warn(string fmt, params object[] args)
        {
            _Log(null, LogLevel.Warn, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Warn(int skipFrames, string fmt, params object[] args)
        {
            _Log(null, LogLevel.Warn, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Err(string fmt, params object[] args)
        {
            _Log(null, LogLevel.Error, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Err(int skipFrames, string fmt, params object[] args)
        {
            _Log(null, LogLevel.Error, _baseSkipFrames + skipFrames, fmt, args);
        }
        /// <summary>
        /// Log Fatal level message.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Fatal(string fmt, params object[] args)
        {
            _Log(null, LogLevel.Fatal, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Fatal(int skipFrames, string fmt, params object[] args)
        {
            _Log(null, LogLevel.Fatal, _baseSkipFrames + skipFrames, fmt, args);
        }
        #endregion // Dbg/Info/Warn/Err/Fatal

        #region Trace<Tag>/Dbg<Tag>/Info<Tag>/Warn<Tag>/Err<Tag>/Fatal<Tag>
        /// <summary>
        /// Log Trace level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Trace<Tag>(string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Trace, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Trace level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Trace<Tag>(int skipFrames, string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Trace, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Dbg<Tag>(string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Debug, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Dbg<Tag>(int skipFrames, string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Debug, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Info<Tag>(string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Info, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Info<Tag>(int skipFrames, string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Info, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Warn<Tag>(string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Warn, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Warn<Tag>(int skipFrames, string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Warn, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Err<Tag>(string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Error, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Err<Tag>(int skipFrames, string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Error, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Fatal<Tag>(string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Fatal, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Fatal<Tag>(int skipFrames, string fmt, params object[] args)
        {
            _Log(typeof(Tag), LogLevel.Fatal, _baseSkipFrames + skipFrames, fmt, args);
        }
        #endregion // Dbg<Tag>/Info<Tag>/Warn<Tag>/Err<Tag>/Fatal<Tag>

        #region Trace2/Dbg2/Info2/Warn2/Err2/Fatal2
        /// <summary>
        /// Log Trace level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Trace2(object tag, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Trace, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Trace level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Trace2(object tag, int skipFrames, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Trace, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Dbg2(object tag, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Debug, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Debug level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Dbg2(object tag, int skipFrames, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Debug, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Info2(object tag, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Info, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Info level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Info2(object tag, int skipFrames, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Info, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Warn2(object tag, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Warn, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Warn level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Warn2(object tag, int skipFrames, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Warn, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Err2(object tag, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Error, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Error level message with tag.
        /// </summary>
        /// <param name="skipFrames">skip frames count</param>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Err2(object tag, int skipFrames, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Error, _baseSkipFrames + skipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Fatal2(object tag, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Fatal, _baseSkipFrames, fmt, args);
        }

        /// <summary>
        /// Log Fatal level message with tag.
        /// </summary>
        /// <param name="fmt">log message format</param>
        /// <param name="args">log message arguments</param>
        public void Fatal2(object tag, int skipFrames, string fmt, params object[] args)
        {
            _Log(tag, LogLevel.Fatal, _baseSkipFrames + skipFrames, fmt, args);
        }
        #endregion // Dbg2/Info2/Warn2/Err2/Fatal2

        #region Internal methods
        private void _Log(object tag, LogLevel level, int skipFrames, string fmt, params object[] args)
        {
            if (!enabled)
                return;

            IntPtr fileName = IntPtr.Zero;
            IntPtr funcName = IntPtr.Zero;
            // Get filename, lineno, funcName, if enabled.
            int lineNo = -1;
            if (_enabledLogFileInfo)
            {
                var st = new System.Diagnostics.StackTrace(true);
                var frame = st.GetFrame(skipFrames);

                if (frame != null)
                {
                    lineNo = frame.GetFileLineNumber();
                    fileName = LibUtil.CreateNativeStr(frame.GetFileName());
                    funcName = LibUtil.CreateNativeStr(frame.GetMethod().Name);
                }
            }

            IntPtr nativeTag = IntPtr.Zero;
            IntPtr nativeMsg = IntPtr.Zero;
            try
            {
                // Get log tag.
                if (tag is Type)
                    nativeTag = LibUtil.CreateNativeStr((tag as Type).Name);
                else if (tag != null)
                    nativeTag = LibUtil.CreateNativeStr(tag.ToString());

                // Build log message.
                nativeMsg = LibUtil.CreateNativeStr(string.Format(fmt, args));
                LLBCNative.csllbc_Log_LogMsg(_nativeLogger, fileName, lineNo, funcName, (int)level, nativeMsg, nativeTag);
            }
            catch (Exception e)
            {
                // Firstly, free nativeMsg.
                System.Runtime.InteropServices.Marshal.FreeHGlobal(nativeMsg);

                // Simply format error message and dump it.
                string errMsg = string.Format("Log [{0}] failed, exception:{1}, stacktrace:\n{2}",
                    fmt, e.Message, new System.Diagnostics.StackTrace().ToString());
                nativeMsg = LibUtil.CreateNativeStr(errMsg);
                LLBCNative.csllbc_Log_LogMsg(_nativeLogger, fileName, lineNo, funcName, (int)LogLevel.Fatal, nativeMsg, nativeTag);
            }
            finally
            {
                System.Runtime.InteropServices.Marshal.FreeHGlobal(nativeTag);
                System.Runtime.InteropServices.Marshal.FreeHGlobal(nativeMsg);
            }

            System.Runtime.InteropServices.Marshal.FreeHGlobal(fileName);
            System.Runtime.InteropServices.Marshal.FreeHGlobal(funcName);
        }
        #endregion // Internal methods

        private string _loggerName;
        private IntPtr _nativeLogger;

        private volatile bool _enabled = true;
        private volatile bool _enabledLogFileInfo;

        private const int _baseSkipFrames = 2;
    }
}
