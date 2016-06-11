/**
 * @file    TestCase_Com_SafeConsole.cs
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/04/22
 * @version 1.0
 *
 * @brief
 */

using System;
using llbc;

class TestCase_Com_SafeConsole : ITestCase
{
    public void Run(string[] args)
    {
        SafeConsole.WriteLine("common/SafeConsole test:");

        SafeConsole.WriteLine("Default foreground color: {0}", SafeConsole.foregroundColor);
        SafeConsole.WriteLine("Default background color: {0}", SafeConsole.backgroundColor);

        for (int i = 0x00; i < 0x10; i++)
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