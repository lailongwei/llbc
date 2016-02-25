/**
 * @file    TestCase_Comm_Service.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */

using System;
using llbc;

class TestCase_Comm_Service : ITestCase
{
    public void Run(string[] args)
    {
        Console.WriteLine("Comm/Service testcase:");

        Service svc = new Service(ServiceType.Normal);

        Console.WriteLine("Press any key to exit...");
        Console.ReadKey();
    }
}