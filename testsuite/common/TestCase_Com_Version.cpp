/**
 * @file    TestCase_Com_Version.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/18
 * @version 1.0
 *
 * @brief
 */

#include "common/TestCase_Com_Version.h"

TestCase_Com_Version::TestCase_Com_Version()
{
}

TestCase_Com_Version::~TestCase_Com_Version()
{
}

int TestCase_Com_Version::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library version test: ");

    LLBC_PrintLine("Major version: %d", LLBC_majorVersion);
    LLBC_PrintLine("Minor version: %d", LLBC_minorVersion);
    LLBC_PrintLine("Update No: %d", LLBC_updateNo);
    LLBC_PrintLine("Is debug version: %s", LLBC_isDebugVer ? "true" : "false");

    LLBC_PrintLine("Version describe(verbose): \n%s", LLBC_GetVersionInfo(true).c_str());

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
