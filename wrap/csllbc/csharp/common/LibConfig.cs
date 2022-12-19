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
using System.Runtime.InteropServices;
using System.Text;

namespace llbc
{
    /// <summary>
    /// llbc library configs definition.
    /// <para>define all llbc core library(c++) config variables</para>
    /// <para>define all csllbc library config variables</para>
    /// </summary>
    public class LibConfig
    {
        #region llbc core library about configs
        #region common configs
        /// <summary>
        /// Default backlog size.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int defaultBacklogSize
        {
            get { return LLBCNative.csllbc_LibConfig_DefaultBacklogSize(); }
        }
        #endregion

        #region logger about configs

        /// <summary>
        /// root logger name.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static string logRootLoggerName
        {
            get
            {
                unsafe
                {
                    byte* loggerName = stackalloc byte[256];
                    int wrote = LLBCNative.csllbc_LibConfig_Log_RootLoggerName((long)loggerName, 256);
                    return new string((sbyte*)loggerName, 0, wrote, Encoding.UTF8);
                }
            }
        }

        /// <summary>
        /// logger config option not config use option("root"/"default").
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static string logDefaultNotConfigOptionUse
        {
            get
            {
                unsafe
                {
                    byte* notConfigUse = stackalloc byte[256];
                    int wrote = LLBCNative.csllbc_LibConfig_Log_DefaultNotConfigOptionUse((long) notConfigUse, 256);
                    return new string((sbyte*)notConfigUse, 0, wrote, Encoding.UTF8);
                }
            }
        }

        /// <summary>
        /// Default log level.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static LogLevel logDefaultLevel
        {
            get { return (LogLevel)LLBCNative.csllbc_LibConfig_Log_DefaultLogLevel(); }
        }

        /// <summary>
        /// Log content direct flush to console or buffer the log content(not-realtime).
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool logDirectFlushToConsole
        {
            get { return LLBCNative.csllbc_LibConfig_Log_IsDirectFlushToConsole() != 0; }
        }

        /// <summary>
        /// Logger default async-mode default config, this config option determine logger running in async-mode or sync-mode.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool logDefaultIsAsyncMode
        {
            get { return LLBCNative.csllbc_LibConfig_Log_DefaultIsAsyncMode() != 0; }
        }

        /// <summary>
        /// Logger default log to console config.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool logDefaultLogToConsole
        {
            get { return LLBCNative.csllbc_LibConfig_Log_DefaultLogToConsole() != 0; }
        }

        /// <summary>
        /// Logger default console log pattern.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static string logDefaultConsoleLogPattern
        {
            get
            {
                unsafe
                {
                    byte *patternBytes = stackalloc byte[256];
                    int wrote = LLBCNative.csllbc_LibConfig_Log_DefaultConsoleLogPattern((long)patternBytes, 256);
                    return new string((sbyte *)patternBytes, 0, wrote, Encoding.UTF8);
                }
            }
        }

        /// <summary>
        /// Logger default colourful-output option enabled or not.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool logDefaultEnabledColourfulOutput
        {
            get { return LLBCNative.csllbc_LibConfig_Log_IsDefaultColourfulOutput() != 0; }
        }

        /// <summary>
        /// Logger default log-to-file option enabled or not.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool logDefaultLogToFile
        {
            get
            { return LLBCNative.csllbc_LibConfig_Log_IsDefaultLogToFile() != 0; }
        }

        /// <summary>
        /// Logger default file log pattern.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static string logDefaultFileLogPattern
        {
            get
            {
                unsafe
                {
                    byte* logPattern = stackalloc byte[256];
                    int wrote = LLBCNative.csllbc_LibConfig_Log_DefaultFileLogPattern((long)logPattern, 256);
                    return new string((sbyte*)logPattern, 0, wrote, Encoding.UTF8);
                }
            }
        }

        /// <summary>
        /// Logger default file rolling mode option.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int logDefaultEnabledDailyMode
        {
            get { return LLBCNative.csllbc_LibConfig_Log_DefaultFileRollingMode(); }
        }

        /// <summary>
        /// Logger default max log file size, in bytes.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static long logDefaultMaxFileSize
        {
            get { return LLBCNative.csllbc_LibConfig_Log_MaxFileSize(); }
        }

        /// <summary>
        /// Logger default max backup index.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int logDefaultMaxBackupIndex
        {
            get { return LLBCNative.csllbc_LibConfig_Log_MaxBackupIndex(); }
        }

        /// <summary>
        /// Logger default log file buffer size, in bytes.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int logDefaultLogFileBufferSize
        {
            get { return LLBCNative.csllbc_LibConfig_Log_DefaultLogFileBufferSize(); }
        }

        /// <summary>
        /// Logger default flush interval, in milli-seconds.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int logDefaultLogFlushInterval
        {
            get { return LLBCNative.csllbc_LibConfig_Log_DefaultLogFlushInterval(); }
        }

        /// <summary>
        /// Logger max flush interval, in milli-seconds.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int logMaxLogFlushInterval
        {
            get { return LLBCNative.csllbc_LibConfig_Log_MaxLogFlushInterval(); }
        }

        /// <summary>
        /// Take over unknown logger message.
        /// </summary>
        public static bool takeOverUnknownLoggerMsg
        {
            get { return LLBCNative.csllbc_LibConfig_Log_TakeOverUnknownLoggerMsg() != 0; }
        }

        /// <summary>
        /// Lazy create log file.
        /// </summary>
        public static bool lazyCreateLogFile
        {
            get { return LLBCNative.csllbc_LibConfig_Log_LazyCreateLogFile() != 0; }
        }
        #endregion // logger about configs

        #region thread about configs
        public static bool debugGuard
        {
            get { return LLBCNative.csllbc_LibConfig_Thread_GuardDebug() != 0; }
        }
        #endregion

        #region timer about configs
        /// <summary>
        /// Timer used strict schedule or not.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool timerStrictSchedule
        {
            get { return LLBCNative.csllbc_LibConfig_Timer_StrictSchedule() != 0; }
        }
        #endregion

        #region Communication about configs
        /// <summary>
        /// Communication byte-order config.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool commIsNetOrder
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_IsNetOrder() != 0; }
        }

        /// <summary>
        /// Communication default connect timeout config, in seconds.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int commDefaultConnectTimeout
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_DefaultConnTimeout(); }
        }

        /// <summary>
        /// Communication max event count, only available in EpollPoller.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int commMaxEventCount
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_MaxEventCount(); }
        }

        /// <summary>
        /// Default service FPS.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int commDefaultServiceFPS
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_DefaultServiceFPS(); }
        }

        /// <summary>
        /// Max service FPS.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int commMaxServiceFPS
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_MaxServiceFPS(); }
        }

        /// <summary>
        /// Per-thread max drive service count.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static int commPerThreadMaxDriveServiceCount
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_PerThreadDriveMaxServiceCount(); }
        }

        /// <summary>
        /// Service Sampler-Support enabled config.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool commIsEnabledSamplerSupport
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_IsEnabledSamplerSupport() != 0; }
        }

        /// <summary>
        /// Service Status-Handler enabled config.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool commIsEnabledStatusHandler
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_IsEnabledStatusHandler() != 0; }
        }

        /// <summary>
        /// Service Status-Desc enabled config.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool commIsEnabledStatusDesc
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_IsEnabledStatusDesc() != 0; }
        }

        /// <summary>
        /// Service Unify-PreSubscribe enabled config.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool commIsEnabledUnifyPreSubscribe
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_IsEnabledUnifyPreSubscribe() != 0; }
        }

        /// <summary>
        /// Service compile used Poller-Model.
        /// <para>Now Supported:</para>
        /// <para>SelectPoller - use select mechanism implement poller(all platforms supported).</para>
        /// <para>IocpPoller   - use Win32 I/O Completion Port mechanism implement poller(only available in Windows platform).</para>
        /// <para>EpollerPoller- use epoll mechanism implement poller(only available in Linux-like platforms).</para>
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static string commPollerModel
        {
            get
            {
                unsafe
                {
                    byte* model = stackalloc byte[32];
                    int wrote = LLBCNative.csllbc_LibConfig_Comm_PollerModel((long)model, 32);
                    return new string((sbyte*)model, 0, wrote, Encoding.UTF8);
                }
            }
        }
        #endregion // Communication about configs
        #endregion // llbc core library about configs
    }
}
