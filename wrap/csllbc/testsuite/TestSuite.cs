/**
 * @file    TestSuite.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 * 
 * @brief
 */

using System;
using System.Collections.Generic;
using System.Text;

using llbc;
using System.Reflection;

namespace csllbc_testsuite
{
    class Program
    {
        static void Main(string[] args)
        {
            LibIniter.Init(Assembly.GetExecutingAssembly());

            ITestCase testCase = null;
            // Common testcases:
            testCase = new TestCase_Com_LibConfig();
            // testCase = new TestCase_Com_SafeConsole();

            // Communication ttestcases:
            // testCase = new TestCase_Comm_Service.TestCase_Comm_Service();

            testCase.Run(args);

            LibIniter.Destroy();
        }
    }
}
