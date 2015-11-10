/**
 * @file    TestCase_Core_Random.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/12
 * @version 1.0
 *
 * @brief
 */

#include "core/random/TestCase_Core_Random.h"

TestCase_Core_Random::TestCase_Core_Random()
{
}

TestCase_Core_Random::~TestCase_Core_Random()
{
}

int TestCase_Core_Random::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/random test:");

    LLBC_PrintLine("RandInt32cmcn() test:");
    for(int i = 0; i < 100; i ++)
    {
        if(i != 0 && i % 10 == 0)
        {
            LLBC_PrintLine("");
        }

        LLBC_PrintLine("%02d ", LLBC_Random::RandInt32cmcn(0, 99));
    }

    LLBC_PrintLine("");
    LLBC_PrintLine("Realc0cn() test:");
    for(int i = 0; i < 100; i ++)
    {
        if(i != 0 && i % 10 == 0)
            LLBC_PrintLine("");

        LLBC_PrintLine("%f ", LLBC_Random::RandRealc0cn(100.0));
    }

    LLBC_PrintLine("");

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}
