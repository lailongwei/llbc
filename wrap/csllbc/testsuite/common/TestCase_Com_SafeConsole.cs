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

class TestCase_Com_SafeConsole : ITestCase
{
    public void Run(string[] args)
    {
        SafeConsole.WriteLine("common/SafeConsole test:");

        SafeConsole.WriteLine("Default foreground color: {0}", SafeConsole.foregroundColor);
        SafeConsole.WriteLine("Default background color: {0}", SafeConsole.backgroundColor);

        for (int i = 0x00; i < 0x10; ++i)
        {
            SafeConsole.WriteLine("Set foreground color to: {0}", (ConsoleColor)i);
            SafeConsole.foregroundColor = (ConsoleColor)i;
            SafeConsole.WriteLine("This is a message ...");
            SafeConsole.ResetColor();

            SafeConsole.WriteLine("Set background color to: {0}", (ConsoleColor)i);
            SafeConsole.backgroundColor = (ConsoleColor)i;
            SafeConsole.WriteLine("This is a message ...");
            SafeConsole.ResetColor();
        }

        SafeConsole.Write("Write bool: ");
        SafeConsole.Write(true);
        SafeConsole.WriteLine(false);

        SafeConsole.Write("Write char: ");
        SafeConsole.Write('a');
        SafeConsole.WriteLine('A');

        SafeConsole.Write("Write object: ");
        SafeConsole.Write(new object());
        SafeConsole.WriteLine(new object());

        SafeConsole.Write("Format write: ");
        SafeConsole.Write("Format write message: {0}", "Hello World!");
        SafeConsole.WriteLine("");

        SafeConsole.WriteLine("Flush call...");
        SafeConsole.Flush();

        SafeConsole.WriteLine("Test WriteErrorLine(): ");
        SafeConsole.WriteErrorLine("Hello World from stderr!");

        SafeConsole.WriteLine("Press any key to exit...");
        Console.Read();
    }
}