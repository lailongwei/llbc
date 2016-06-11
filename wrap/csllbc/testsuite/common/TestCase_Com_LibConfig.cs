/**
 * @file    TestCase_Com_LibConfig.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/21
 * @version 1.0
 *
 * @brief
 */

using System;
using llbc;

class TestCase_Com_LibConfig : ITestCase
{
    public void Run(string[] args)
    {
        Console.WriteLine("Common/LibConfig testcase:");

        Console.WriteLine("Common about configs:");
        Console.WriteLine("Default backlog size: {0}", LibConfig.defaultBacklogSize);
        Console.WriteLine();

        Console.WriteLine("Log about configs:");
        Console.WriteLine("Log default level: {0}", LibConfig.logDefaultLevel);
        Console.WriteLine("Log direct flush to console: {0}", LibConfig.logDirectFlushToConsole);
        Console.WriteLine("Log default is async-mode: {0}", LibConfig.logDefaultIsAsyncMode);
        Console.WriteLine("Log default console log pattern: {0}", LibConfig.logDefaultConsoleLogPattern);
        Console.WriteLine("Log default enabled colourful output: {0}", LibConfig.logDefaultEnabledColourfulOutput);
        Console.WriteLine("Log default log to file: {0}", LibConfig.logDefaultLogToFile);
        Console.WriteLine("Log default log file name: {0}", LibConfig.logDefaultLogFileName);
        Console.WriteLine("Log default file log pattern: {0}", LibConfig.logDefaultFileLogPattern);
        Console.WriteLine("Log default enabled daily-rolling mode: {0}", LibConfig.logDefaultEnabledDailyMode);
        Console.WriteLine("Log default max log file size(in bytes): {0}", LibConfig.logDefaultMaxFileSize);
        Console.WriteLine("Log default max backup index: {0}", LibConfig.logDefaultMaxBackupIndex);
        Console.WriteLine("Log default log file buffer size(in bytes): {0}", LibConfig.logDefaultLogFileBufferSize);
        Console.WriteLine("Log default log flush interval(milli-seconds): {0}", LibConfig.logDefaultLogFlushInterval);
        Console.WriteLine("Log max log flush interval(milli-seconds): {0}", LibConfig.logMaxLogFlushInterval);
        Console.WriteLine();

        Console.WriteLine("Timer about configs:");
        Console.WriteLine("Used strict scuedule: {0}", LibConfig.timerStrictSchedule);
        Console.WriteLine();

        Console.WriteLine("Communication about configs:");
        Console.WriteLine("Comm default connect timeout(seconds): {0}", LibConfig.commDefaultConnectTimeout);
        Console.WriteLine("Comm max event count: {0}", LibConfig.commMaxEventCount);
        Console.WriteLine("Comm default service FPS: {0}", LibConfig.commDefaultServiceFPS);
        Console.WriteLine("Comm max service FPS: {0}", LibConfig.commMaxServiceFPS);
        Console.WriteLine("Comm per-thread max drive service count: {0}", LibConfig.commPerThreadMaxDriveServiceCount);
        Console.WriteLine("Comm enabled sampler support: {0}", LibConfig.commIsEnabledSamplerSupport);
        Console.WriteLine("Comm used full stack: {0}", LibConfig.commIsUseFullStack);
        Console.WriteLine("Comm enabled status handler: {0}", LibConfig.commIsEnabledStatusHandler);
        Console.WriteLine("Comm enabled status desc: {0}", LibConfig.commIsEnabledStatusDesc);
        Console.WriteLine("Comm enabled unify pre-subscribe: {0}", LibConfig.commIsEnabledUnifyPreSubscribe);
        Console.WriteLine("Comm poller model: {0}", LibConfig.commPollerModel);
        Console.WriteLine();

        Console.WriteLine("Press any key ton continue...");
        Console.ReadKey();
    }
}