/**
 * @file    LibConfig.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/26
 * @version 1.0
 *
 * @brief
 */

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
            get { return LLBCNative.csllbc_LibConfig_Log_IsDeafaultEnabledColourfulOutput() != 0; }
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
        /// Logger default log file name.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static string logDefaultLogFileName
        {
            get
            {
                unsafe
                {
                    byte* fileName = stackalloc byte[256];
                    int wrote = LLBCNative.csllbc_LibConfig_Log_DefaultLogFileName((long)fileName, 256);
                    return new string((sbyte*)fileName, 0, wrote, Encoding.UTF8);
                }
            }
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
        /// Logger default daily-rolling mode option.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool logDefaultEnabledDailyMode
        {
            get { return LLBCNative.csllbc_LibConfig_Log_IsDefaultEnabledDailyMode() != 0; }
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
        /// Service Full-Stack Used config.
        /// <para>config layer: llbc core library</para>
        /// </summary>
        public static bool commIsUseFullStack
        {
            get { return LLBCNative.csllbc_LibConfig_Comm_IsUsedFullStack() != 0; }
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
