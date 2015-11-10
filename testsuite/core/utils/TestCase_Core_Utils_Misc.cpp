/**
 * @file    TestCase_Core_Utils_Misc.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/05
 * @version 1.0
 *
 * @brief
 */

#include "core/utils/TestCase_Core_Utils_Misc.h"

namespace
{
    void GuardFunc(void *data)
    {
        std::cout <<"Test Guard Function Called!" <<std::endl;
        std::cout <<"  The data argument: " <<data <<std::endl;
    }

    class GuardCls
    {
    public:
        void GuardMeth(void *data)
        {
            std::cout <<"Test Guard Method Called!" <<std::endl;
            std::cout <<"  The data argument: " <<data <<std::endl;
        }
    };
}

int TestCase_Core_Utils_Misc::Run(int argc, char *argv[])
{
    std::cout <<"core/utils/Util_Misc test:" <<std::endl;
    this->TestGuardFunc();
    this->TestGuardMeth();

    std::cout <<"Press any key to continue...";
    getchar();

    return LLBC_RTN_OK;
}

void TestCase_Core_Utils_Misc::TestGuardFunc()
{
    std::cout <<"Guard Function test:" <<std::endl;
    LLBC_InvokeGuard guard(&GuardFunc, (void *)0x01);
}

void TestCase_Core_Utils_Misc::TestGuardMeth()
{
    std::cout <<"Guard Method test:" <<std::endl;
    GuardCls obj;
    {
        LLBC_InvokeGuard guard(&obj, &GuardCls::GuardMeth, (void *)0x02);
    }
}
