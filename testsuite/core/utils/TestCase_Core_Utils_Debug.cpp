/**
* @file     TestCase_Core_Utils_Debug.cpp
* @author   Longwei Lai<lailongwei@126.com>
* @date     2013/04/04
* @version  1.0
*
* @brief
*/

#include "core/utils/TestCase_Core_Utils_Debug.h"

int TestCase_Core_Utils_Debug::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/utils/Util_Debug test:");

    // Byte2Hex test.
    char str[256];
    for(int i = 0; i < 256; i ++)
    {
        str[i] = static_cast<char>(i);
    }

    LLBC_PrintLine("Byte2Hex test:\n%s", LLBC_Byte2Hex(str, 256, 32).c_str());
    LLBC_PrintLine("");

    // CPUTime test.
    LLBC_CPUTime begin = LLBC_CPUTime::Current();
    for(int i = 0; i < 10000000; ++ i)
    {
    }
    LLBC_CPUTime diff = LLBC_CPUTime::Current() - begin;
    LLBC_PrintLine("CPUTime, diff time: %s", diff.ToString().c_str());
    LLBC_PrintLine("");

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
