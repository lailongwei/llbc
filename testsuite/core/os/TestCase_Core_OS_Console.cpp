/**
 * @file    TestCase_Core_OS_Console.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/05/05
 * @version 1.0
 *
 * @brief
 */

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
