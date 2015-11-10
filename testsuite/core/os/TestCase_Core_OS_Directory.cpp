/**
 * @file    TestCase_Core_OS_Directory.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/24
 * @version 1.0
 *
 * @brief
 */

#include "core/os/TestCase_Core_OS_Directory.h"

TestCase_Core_OS_Directory::TestCase_Core_OS_Directory()
{
}

TestCase_Core_OS_Directory::~TestCase_Core_OS_Directory()
{
}

int TestCase_Core_OS_Directory::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/os/directory test:");

    LLBC_PrintLine("Current working directory: %s", LLBC_GetCurrentDirectory().c_str() );
    LLBC_PrintLine("Home directory: %s", LLBC_GetHomeDirectory().c_str() );
    LLBC_PrintLine("Document directory: %s", LLBC_GetDocumentDirectory().c_str() );
    LLBC_PrintLine("Temporary directory: %s", LLBC_GetTemporaryDirectory().c_str() );
    LLBC_PrintLine("Cache directory: %s", LLBC_GetCacheDirectory().c_str() );

    LLBC_PrintLine("Join 'a/b' and 'c/d: %s", LLBC_JoinDirectory("a/b", "c/d").c_str());
    LLBC_PrintLine("Join 'a/b/' and 'c/d: %s", LLBC_JoinDirectory("a/b/", "c/d").c_str());
    LLBC_PrintLine("Join 'a/b' and '/c/d: %s", LLBC_JoinDirectory("a/b", "/c/d").c_str());
    LLBC_PrintLine("Join 'a/b/' and '/c/d: %s", LLBC_JoinDirectory("a/b/", "/c/d").c_str());

    LLBC_PrintLine("Join 'a', 'b', 'c': %s", LLBC_JoinDirectory("a", "b", "c").c_str());
    LLBC_PrintLine("Join 'a', 'b', 'c', 'd': %s", LLBC_JoinDirectory("a", "b", "c", "d").c_str());
    LLBC_PrintLine("Join 'a', 'b', 'c', 'd', 'e': %s", LLBC_JoinDirectory("a", "b", "c", "d", "e").c_str());
    LLBC_PrintLine("Join 'a', 'b', 'c', 'd', 'e', 'f': %s", LLBC_JoinDirectory("a", "b", "c", "d", "e", "f").c_str());
    LLBC_PrintLine("Join 'a', 'b', 'c', 'd', 'e', 'f', 'g': %s",
        LLBC_JoinDirectory("a", "b", "c", "d", "e", "f", "g").c_str());

    LLBC_PrintLine("Press any key to exit ... ...");
    getchar();

    return 0;
}

