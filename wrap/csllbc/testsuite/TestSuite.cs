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

namespace csllbc_testsuite
{
    class Program
    {
        static void Main(string[] args)
        {
            LibIniter.Init();

            ITestCase testCase = null;
            testCase = new TestCase_Comm_Service();
            testCase.Run(args);

            LibIniter.Destroy();
        }
    }
}
