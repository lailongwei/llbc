/**
 * @file    TestCase_Com_Compiler.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/26
 * @version 1.0
 *
 * @brief
 */

#include "common/TestCase_Com_Compiler.h"

TestCase_Com_Compiler::TestCase_Com_Compiler()
{
}

TestCase_Com_Compiler::~TestCase_Com_Compiler()
{
}

int TestCase_Com_Compiler::Run(int argc, char *argv[])
{
    LLBC_PrintLine("common/compiler test:");

    LLBC_String compInfo;
    compInfo.append_format("  Compiler Type: %d\n", LLBC_CUR_COMP)
        .append_format("  Compiler Version: %d\n", LLBC_COMP_VER)
        .append_format("  Major Version: %d\n", LLBC_COMP_MAJOR_VER)
        .append_format("  Minor Version: %d\n", LLBC_COMP_MINOR_VER)
        .append_format("  Patch Level: %d\n", LLBC_COMP_PATCH_LEVEL);

    LLBC_PrintLine("%s", compInfo.c_str());

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    return LLBC_OK;
}

