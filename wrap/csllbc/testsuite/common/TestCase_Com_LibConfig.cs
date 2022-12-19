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
using llbc;

class TestCase_Com_LibConfig : ITestCase
{
    public void Run(string[] args)
    {
        Console.WriteLine("Common/LibConfig testcase:");

        Console.WriteLine("Common about configs:");
        Console.WriteLine("  Default backlog size: {0}", LibConfig.defaultBacklogSize);
        Console.WriteLine();

        Console.WriteLine("Log about configs:");
        Console.WriteLine("  Log root logger name: {0}", LibConfig.logRootLoggerName);
        Console.WriteLine("  Log default not config option use: {0}", LibConfig.logDefaultNotConfigOptionUse);
        Console.WriteLine("  Log default level: {0}", LibConfig.logDefaultLevel);
        Console.WriteLine("  Log direct flush to console: {0}", LibConfig.logDirectFlushToConsole);
        Console.WriteLine("  Log default is async-mode: {0}", LibConfig.logDefaultIsAsyncMode);
        Console.WriteLine("  Log default console log pattern: {0}", LibConfig.logDefaultConsoleLogPattern);
        Console.WriteLine("  Log default colourful output: {0}", LibConfig.logDefaultEnabledColourfulOutput);
        Console.WriteLine("  Log default log to file: {0}", LibConfig.logDefaultLogToFile);
        Console.WriteLine("  Log default file log pattern: {0}", LibConfig.logDefaultFileLogPattern);
        Console.WriteLine("  Log default lazy create log file: {0}", LibConfig.lazyCreateLogFile);
        Console.WriteLine("  Log default file rolling mode: {0}", LibConfig.logDefaultEnabledDailyMode);
        Console.WriteLine("  Log default max log file size(in bytes): {0}", LibConfig.logDefaultMaxFileSize);
        Console.WriteLine("  Log default max backup index: {0}", LibConfig.logDefaultMaxBackupIndex);
        Console.WriteLine("  Log default log file buffer size(in bytes): {0}", LibConfig.logDefaultLogFileBufferSize);
        Console.WriteLine("  Log default log flush interval(milli-seconds): {0}", LibConfig.logDefaultLogFlushInterval);
        Console.WriteLine("  Log max log flush interval(milli-seconds): {0}", LibConfig.logMaxLogFlushInterval);
        Console.WriteLine("  Log take over unknown logger message: {0}", LibConfig.takeOverUnknownLoggerMsg);
        Console.WriteLine();

        Console.WriteLine("Thread about configs:");
        Console.WriteLine("  Enable guard debug option?: {0}", LibConfig.debugGuard);
        Console.WriteLine();

        Console.WriteLine("Timer about configs:");
        Console.WriteLine("  Used strict scuedule: {0}", LibConfig.timerStrictSchedule);
        Console.WriteLine();

        Console.WriteLine("Communication about configs:");
        Console.WriteLine("  Comm default connect timeout(seconds): {0}", LibConfig.commDefaultConnectTimeout);
        Console.WriteLine("  Comm max event count: {0}", LibConfig.commMaxEventCount);
        Console.WriteLine("  Comm default service FPS: {0}", LibConfig.commDefaultServiceFPS);
        Console.WriteLine("  Comm max service FPS: {0}", LibConfig.commMaxServiceFPS);
        Console.WriteLine("  Comm per-thread max drive service count: {0}", LibConfig.commPerThreadMaxDriveServiceCount);
        Console.WriteLine("  Comm enabled sampler support: {0}", LibConfig.commIsEnabledSamplerSupport);
        Console.WriteLine("  Comm enabled status handler: {0}", LibConfig.commIsEnabledStatusHandler);
        Console.WriteLine("  Comm enabled status desc: {0}", LibConfig.commIsEnabledStatusDesc);
        Console.WriteLine("  Comm enabled unify pre-subscribe: {0}", LibConfig.commIsEnabledUnifyPreSubscribe);
        Console.WriteLine("  Comm poller model: {0}", LibConfig.commPollerModel);
        Console.WriteLine();

        Console.WriteLine("Press any key ton continue...");
        Console.ReadKey();
    }
}
