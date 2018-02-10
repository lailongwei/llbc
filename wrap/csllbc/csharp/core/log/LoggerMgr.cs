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
    public class LoggerMgr
    {
        #region inited
        public static bool inited
        {
            get { return LLBCNative.csllbc_Log_IsInited() != 0; }
        }
        #endregion

        #region Init/Destroy
        /// <summary>
        /// Initialize Log.
        /// </summary>
        /// <param name="logCfgFile">log config file</param>
        public static void Init(string logCfgFile)
        {
            if (string.IsNullOrEmpty(logCfgFile))
                throw new LLBCException("please specific log config file");

            lock (_lock)
            {
                IntPtr nativeStr = LibUtil.CreateNativeStr(logCfgFile);
                int ret = LLBCNative.csllbc_Log_Init(nativeStr);
                System.Runtime.InteropServices.Marshal.FreeHGlobal(nativeStr);

                if (ret != LLBCNative.LLBC_OK)
                    throw ExceptionUtil.CreateExceptionFromCoreLib();

                _rootLogger = Get("root");
            }
        }

        /// <summary>
        /// Destroy log config file.
        /// </summary>
        public static void Destroy()
        {
            lock (_lock)
            {
                _loggers.Clear();
                _rootLogger = null;
            }

            LLBCNative.csllbc_Log_Destroy();
        }

        public static Logger Get(string loggerName)
        {
            lock (_lock)
            {
                Logger logger;
                if (!_loggers.TryGetValue(loggerName, out logger))
                    _loggers.Add(loggerName, (logger = new Logger(loggerName)));
                return logger;
            }
        }

        public static Logger GetRoot()
        {
            lock (_lock)
            {
                return _rootLogger;
            }
        }
        #endregion // Init/Destroy

        private static object _lock = new object();

        private static Logger _rootLogger;
        private static System.Collections.Generic.Dictionary<string, Logger> _loggers = 
            new System.Collections.Generic.Dictionary<string,Logger>();
    }
}