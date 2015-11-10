/**
 * @file    TestCase_Core_Utils_Algorithm.cpp
 * @author  Longwei Lai
 * @date    2013/04/01
 * @version 1.0
 *
 * @brief
 */

#include "core/utils/TestCase_Core_Utils_Algorithm.h"

int TestCase_Core_Utils_Algorithm::Run(int argc, char *argv[])
{
    LLBC_PrintLine("llbc library core/utils/Util_Algorithm test");

    LLBC_PrintLine("flow check test:");
    LLBC_PrintLine("3 + 4: %s", 
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseAdd(3, 4)));
    LLBC_PrintLine("(sint16)32767 + (sint16)2: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseAdd( (sint16)32767, (sint16)2)));
    LLBC_PrintLine("(sint16)-32768 + (sint16)-2: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseAdd( (sint16)-32768, (sint16)-2)));

    LLBC_PrintLine("2 - 1: %s", 
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseSub(2, 1)));
    LLBC_PrintLine("(sint16)-32768 - (sing16)1: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseSub( (sint16)-32768, (sint16)1)));
    LLBC_PrintLine("(sint16)32767 - (sint16)-2: %s",
        LLBC_FlowType::Type2Str( LLBC_CheckFlowUseSub( (sint16)32767, (sint16)-2)));
    LLBC_PrintLine("");

    LLBC_PrintLine("NOFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::NoFlow));
    LLBC_PrintLine("UNDERFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::UnderFlow));
    LLBC_PrintLine("OVERFLOW string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::OverFlow));
    LLBC_PrintLine("UNKNOWN string: %s", LLBC_FlowType::Type2Str(LLBC_FlowType::Unknown));
    LLBC_PrintLine("0xff3d string: %s", LLBC_FlowType::Type2Str(0xff3d));

    LLBC_PrintLine("test completed, press any key to exit");

    getchar();

    return 0;
}

