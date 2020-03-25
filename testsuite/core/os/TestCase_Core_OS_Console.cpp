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


#include "core/os/TestCase_Core_OS_Console.h"

TestCase_Core_OS_Console::TestCase_Core_OS_Console()
{
}

TestCase_Core_OS_Console::~TestCase_Core_OS_Console()
{
}

int TestCase_Core_OS_Console::Run(int argc, char *argv[])
{
    std::cout <<"core/os/console test: " <<std::endl;

    // Set console color.
    int defaultClr = LLBC_GetConsoleColor(stdout);
    std::cout <<"Current console color: " <<LLBC_GetConsoleColor(stdout) <<std::endl;

    // test foreground color.
    std::cout <<"foreground color test: " <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Default);
    std::cout <<"stdout, Fg_Red | Bg_Default" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Green | LLBC_ConsoleColor::Bg_Default);
    std::cout <<"stdout, Fg_Green | Bg_Default" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Blue | LLBC_ConsoleColor::Bg_Default);
    std::cout <<"stdout, Fg_Blue | Bg_Default" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Yellow | LLBC_ConsoleColor::Bg_Default);
    std::cout <<"stdout, Fg_Yellow | Bg_Default" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Purple | LLBC_ConsoleColor::Bg_Default);
    std::cout <<"stdout, Fg_Purple | Bg_Default" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Cyan| LLBC_ConsoleColor::Bg_Default);
    std::cout <<"stdout, Fg_Cyan | Bg_Default" <<std::endl;
    std::cout <<std::endl;

    // test background color.
    LLBC_SetConsoleColor(stdout, defaultClr);
    std::cout <<"background color test: " <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Red);
    std::cout <<"stdout, Fg_Default | Bg_Red" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Green);
    std::cout <<"stdout, Fg_Default | Bg_Green" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Blue);
    std::cout <<"stdout, Fg_Default | Bg_Blue" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Yellow);
    std::cout <<"stdout, Fg_Default | Bg_Yellow" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Purple);
    std::cout <<"stdout, Fg_Default | Bg_Purple" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Cyan);
    std::cout <<"stdout, Fg_Default | Bg_Cyan" <<std::endl;
    std::cout <<std::endl;

	// test foreground color.
    LLBC_PrintLine("foreground color test: ");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Default);
    LLBC_PrintLine("stdout, Fg_Red | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Green | LLBC_ConsoleColor::Bg_Default);
    LLBC_PrintLine("stdout, Fg_Green | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Blue | LLBC_ConsoleColor::Bg_Default);
    LLBC_PrintLine("stdout, Fg_Blue | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Yellow | LLBC_ConsoleColor::Bg_Default);
    LLBC_PrintLine("stdout, Fg_Yellow | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Purple | LLBC_ConsoleColor::Bg_Default);
    LLBC_PrintLine("stdout, Fg_Purple | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Cyan | LLBC_ConsoleColor::Bg_Default);
    LLBC_PrintLine("stdout, Fg_Cyan | Bg_Default");
    LLBC_PrintLine("");

    // test background color.
    LLBC_SetConsoleColor(stdout, defaultClr);
    LLBC_PrintLine("background color test: ");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Red);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Red");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Green);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Green");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Blue);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Blue");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Yellow);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Yellow");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Purple);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Purple");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Cyan);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Cyan");
    LLBC_PrintLine("");

	// test highlight foreground color.
    LLBC_PrintLine("highlight foreground color test: ");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Default | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Red | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Green | LLBC_ConsoleColor::Bg_Default | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Green | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Blue | LLBC_ConsoleColor::Bg_Default | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Blue | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Yellow | LLBC_ConsoleColor::Bg_Default | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Yellow | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Purple | LLBC_ConsoleColor::Bg_Default | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Purple | Bg_Default");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Cyan | LLBC_ConsoleColor::Bg_Default | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Cyan | Bg_Default");
    LLBC_PrintLine("");

    // test highlight background color.
    LLBC_SetConsoleColor(stdout, defaultClr);
    LLBC_PrintLine("highlight background color test: ");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Red | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Red");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Green | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Green");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Blue | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Blue");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Yellow | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Yellow");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Purple | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Purple");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Default | LLBC_ConsoleColor::Bg_Cyan | LLBC_ConsoleColor::Highlight_Fg);
    LLBC_PrintLine("stdout, Fg_Default | Bg_Cyan");
    LLBC_PrintLine("");

    // test highlight.
    LLBC_SetConsoleColor(stdout, defaultClr);
    std::cout <<"highlight test: " <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Yellow);
    std::cout <<"Normal" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | 
        LLBC_ConsoleColor::Bg_Yellow | LLBC_ConsoleColor::Highlight_Fg);
    std::cout <<"Highlight foreground" <<std::endl;
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | 
        LLBC_ConsoleColor::Bg_Yellow | LLBC_ConsoleColor::Highlight_Bg);
    std::cout <<"Highlight background" <<std::endl;
    LLBC_SetConsoleColor(stdout, 
        LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Yellow | 
        LLBC_ConsoleColor::Highlight_Fg | LLBC_ConsoleColor::Highlight_Bg);
    std::cout <<"Highlight foreground & background" <<std::endl;

	LLBC_SetConsoleColor(stdout, defaultClr);
   LLBC_PrintLine("highlight test: ");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Yellow);
   LLBC_PrintLine("Normal");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Yellow | LLBC_ConsoleColor::Highlight_Fg);
   LLBC_PrintLine("Highlight foreground");
    LLBC_SetConsoleColor(stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Yellow | LLBC_ConsoleColor::Highlight_Bg);
   LLBC_PrintLine("Highlight background");
    LLBC_SetConsoleColor(
        stdout, LLBC_ConsoleColor::Fg_Red | LLBC_ConsoleColor::Bg_Yellow | LLBC_ConsoleColor::Highlight_Fg | LLBC_ConsoleColor::Highlight_Bg);
   LLBC_PrintLine("Highlight foreground & background");

    LLBC_SetConsoleColor(stderr, defaultClr);

    // print family apis test.
    std::cout <<std::endl;
    std::cout <<"====================================================" <<std::endl;
    std::cout <<"Print family apis test:" <<std::endl;
    const LLBC_String str = "Hello World\n";
    const LLBC_String hrStr = "Hello World\\n";
    std::cout <<"LLBC_Print(\"" <<hrStr <<"\"):" <<std::endl;
    LLBC_Print(str.c_str());

    std::cout <<"LLBC_PrintLine(\"" <<hrStr <<"\"):" <<std::endl;
    LLBC_PrintLine(str.c_str());

    std::cout <<"LLBC_FilePrint(stdout, \"" <<hrStr <<"\"):" <<std::endl;
    LLBC_FilePrint(stdout, str.c_str());

    std::cout <<"LLBC_FilePrintLine(stdout, \"" <<hrStr <<"\"):" <<std::endl;
    LLBC_FilePrintLine(stdout, str.c_str());
    std::cout <<"====================================================" <<std::endl;

    fprintf(stdout, "Press any key to continue ... ...\n");
    getchar();

    return 0;
}
