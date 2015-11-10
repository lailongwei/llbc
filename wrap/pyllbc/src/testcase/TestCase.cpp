/**
 * @file    TestCase.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/13
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc/testcase/TestCase.h"

#include "src/testcase/_TestcaseMethods.h"

void pyllbc_InitTestCase()
{
}

void pyllbc_DestroyTestCase()
{
}

void pyllbc_AddTestCaseMethods()
{
    pyllbc_TestcaseMethods &methods = *pyllbc_s_TestcaseMethods;

    pyllbc_Module *inl = pyllbc_InlModule;
    inl->AddMethod(methods.NewTestCase);
    inl->AddMethod(methods.DelTestCase);
    inl->AddMethod(methods.GetTestCaseName);
    inl->AddMethod(methods.SetTestCaseName);
    inl->AddMethod(methods.RunTestCase);
}

void pyllbc_AddTestCaseObjs()
{
}
