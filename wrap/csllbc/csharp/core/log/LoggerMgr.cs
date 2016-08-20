/**
 * @file    LoggerMgr.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/07/10
 * @version 1.0
 *
 * @brief
 */

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