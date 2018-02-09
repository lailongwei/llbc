/**
 * @file    TestCase_Com_RTTI.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/09
 * @version 1.0
 *
 * @brief
 */

#include "common/TestCase_Com_RTTI.h"

namespace
{
    struct UserDefineStruct
    {
        int val1;
        LLBC_String val2;
    };

    class UserDefineClass
    {
        int val1;
        LLBC_String val2;
    };
}

TestCase_Com_RTTI::TestCase_Com_RTTI()
{
}

TestCase_Com_RTTI::~TestCase_Com_RTTI()
{
}

int TestCase_Com_RTTI::Run(int argc, char *argv[])
{
    LLBC_PrintLine("Common/RTTI test:");

    LLBC_PrintLine("Raw type name test:");
    LLBC_PrintLine("  LLBC_GetTypeName(bool): %s", LLBC_GetTypeName(bool).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(sint8): %s", LLBC_GetTypeName(sint8).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(uint8): %s", LLBC_GetTypeName(uint8).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(sint16): %s", LLBC_GetTypeName(sint16).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(uint16): %s", LLBC_GetTypeName(uint16).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(sint32): %s", LLBC_GetTypeName(sint32).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(uint32): %s", LLBC_GetTypeName(uint32).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(sint64): %s", LLBC_GetTypeName(sint64).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(uint64): %s", LLBC_GetTypeName(uint64).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(float): %s", LLBC_GetTypeName(float).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(double): %s", LLBC_GetTypeName(double).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(std::string): %s", LLBC_GetTypeName(std::string).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(LLBC_String): %s", LLBC_GetTypeName(LLBC_String).c_str());

    LLBC_PrintLine("\nPoint type test:");
    LLBC_PrintLine("  LLBC_GetTypeName((int *)): %s", LLBC_GetTypeName(int*).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName((int *)NULL): %s", LLBC_GetTypeName((int*)NULL).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(int **): %s", LLBC_GetTypeName(int**).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName((int **)NULL): %s", LLBC_GetTypeName((int**)NULL).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName((int ***)NULL): %s", LLBC_GetTypeName((int***)NULL).c_str());

    LLBC_PrintLine("\nUser defined classes/structs test:");
    LLBC_PrintLine("  LLBC_GetTypeName(UserDefineStruct): %s", LLBC_GetTypeName(UserDefineStruct).c_str());
    LLBC_PrintLine("  LLBC_GetTypeName(UserDefineClass): %s", LLBC_GetTypeName(UserDefineClass).c_str());

    LLBC_PrintLine("Press any key to continue...");
    getchar();

    return LLBC_OK;
}


