/**
 * @file    TestCase_Core_Config_Ini.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/09/10
 * @version 1.0
 *
 * @brief
 */

using System;
using llbc;
using Console = llbc.SafeConsole;

class TestCase_Core_Config_Ini : ITestCase
{
    public void Run(string[] args)
    {
        Console.WriteLine("Core/Config/Ini test:");

        using (var ini = new Ini())
        {
            string file = "test_ini.ini";
            Console.WriteLine("Load config file: {0}", file);
            ini.LoadFromFile(file);

            var helloSection = ini["Hello"];
            Console.WriteLine("Hello section get: {0}", helloSection.sectionName);

            var unknownSection = ini.GetSection("sdfsafafas");
            Console.WriteLine("Get not exist section, return: {0}", unknownSection);

            Console.WriteLine("[Hello] Cfg1 = {0}", ini.GetValue<int>("Hello", "Cfg1"));
            Console.WriteLine("[Hello] Cfg2 = {0}", ini.GetValue<string>("Hello", "Cfg2"));
            Console.WriteLine("[Hello] Cfg3 = {0}", ini.GetValue<string>("Hello", "Cfg3"));
            Console.WriteLine("[Hello] Cfg4 = {0}", ini.GetValue<bool>("Hello", "Cfg4"));
            
            Console.WriteLine("[World] Cfg1 = {0}", ini.GetValue<int>("World", "Cfg1"));
            Console.WriteLine("[World] Cfg2 = {0}", ini.GetValue<string>("World", "Cfg2"));
            Console.WriteLine("[World] Cfg3 = {0}", ini.GetValue<string>("World", "Cfg3"));
            Console.WriteLine("[World] Cfg4 = {0}", ini.GetValue<bool>("World", "Cfg4"));
        }

        Console.WriteLine("Press any key to exit...");
        Console.ReadKey();
    }
}