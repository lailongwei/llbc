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