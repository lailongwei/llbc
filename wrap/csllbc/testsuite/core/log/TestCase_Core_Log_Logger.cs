/**
 * @file    TestCase_Core_Log_Logger.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/07/10
 * @version 1.0
 *
 * @brief
 */

using System;
using llbc;
using Console = llbc.SafeConsole;

class TestCase_Core_Log_Logger : ITestCase
{
    public void Run(string[] args)
    {
        Console.WriteLine("Core/Log/Logger test:");

        string logCfg = "Logger_Cfg.cfg";
        LoggerMgr.Init(logCfg);

        // Test Root logger.
        Log.enabledLogFileInfo = true;
        Log.Dbg("A debug message from Log.Dbg()...");
        Log.Info("A info message from Log.Info()...");
        Log.Warn("A warn message from Log.Warn()...");
        Log.Err("A error message from Log.Err()...");
        Log.Fatal("A fatal message from Log.Fatal()...");

        Log.Dbg<Log>("A debug message from Log.Dbg<Log>()...");
        Log.Info<Log>("A debug message from Log.Dbg<Log>()...");
        Log.Warn<Log>("A debug message from Log.Dbg<Log>()...");
        Log.Err<Log>("A debug message from Log.Dbg<Log>()...");
        Log.Fatal<Log>("A debug message from Log.Dbg<Log>()...");

        // Get test logger to test.
        var testLogger = LoggerMgr.Get("test");
        testLogger.Dbg("A debug message from testLogger.Dbg()...");
        testLogger.Info("A debug message from testLogger.Dbg()...");
        testLogger.Warn("A debug message from testLogger.Dbg()...");
        testLogger.Err("A debug message from testLogger.Dbg()...");
        testLogger.Fatal("A debug message from testLogger.Dbg()...");

        Console.WriteLine("Press any key to exit test...");
        Console.ReadKey();

        LoggerMgr.Destroy();
    }
}