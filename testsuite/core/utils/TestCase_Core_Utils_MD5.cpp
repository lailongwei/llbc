/**
 * @file    TestCase_Core_Utils_MD5.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2103/06/12
 * @version 1.0
 *
 * @brief
 */

#include "core/utils/TestCase_Core_Utils_MD5.h"

TestCase_Core_Utils_MD5::TestCase_Core_Utils_MD5()
{
}

TestCase_Core_Utils_MD5::~TestCase_Core_Utils_MD5()
{
}

int TestCase_Core_Utils_MD5::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/utils/md5 test:");

    LLBC_String str1;
    LLBC_String str2 = "Hello World";
    LLBC_String str3 = "Hello WorldHello WorldHello WorldHello WorldHello WorldHello WorldHello World";

    // d41d8cd98f00b204e9800998ecf8427e
    LLBC_PrintLine("Content: %s", str1.c_str());
    LLBC_PrintLine("MD5: %s",  LLBC_MD5::MD5_String(str1.c_str()).c_str());

    // b10a8db164e0754105b7a99be72e3fe5
    LLBC_PrintLine("Content: %s", str2.c_str());
    LLBC_PrintLine("MD5: %s", LLBC_MD5::MD5_String(str2.c_str()).c_str());

    // f90ca33af8bc681c9affec37711e7593
    LLBC_PrintLine("Content: %s", str3.c_str());
    LLBC_PrintLine("Content: %s", LLBC_MD5::MD5_String(str3.c_str()).c_str());

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
