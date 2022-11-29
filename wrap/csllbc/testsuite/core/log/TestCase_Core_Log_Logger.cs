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
using Console = llbc.SafeConsole;

class TestCase_Core_Log_Logger : ITestCase
{
    public void Run(string[] args)
    {
        Console.WriteLine("Core/Log/Logger test:");

        string logCfg = "LoggerCfg.cfg";
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
        testLogger.Info("A info message from testLogger.Info()...");
        testLogger.Warn("A warn message from testLogger.Warn()...");
        testLogger.Err("A error message from testLogger.Err()...");
        testLogger.Fatal("A fatal message from testLogger.Fatal()...");

        Console.WriteLine("Press any key to exit test...");
        Console.ReadKey();

        LoggerMgr.Destroy();
    }
}
