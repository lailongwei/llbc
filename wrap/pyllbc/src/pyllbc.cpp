/**
 * @file    pyllbc.cpp
 * @author  Longwei Lai
 * @date    2014/07/29
 * @version 1.0
 *
 * @brief
 */

#include "pyllbc/common/Export.h"

#include "pyllbc.h"
#include "src/pyllbc_scripts.h"

/**
 * model init method.
 */
#ifndef LLBC_DEBUG
void initllbc()
#else // defined LLBC_DEBUG macro
void initllbc_debug()
#endif // not define LLBC_DEBUG
{
    /* Init llbc library. */
    if (LLBC_Startup() == LLBC_FAILED)
    {
        if (LLBC_GetLastError() != LLBC_ERROR_REENTRY)
        {
            LLBC_String errStr;
            errStr.format("Initialize llbc library failed, err: %s", LLBC_FormatLastError());

            PyErr_SetString(PyExc_Exception, errStr.c_str());

            return;
        }
    }

    /* Init pyllbc modules. */
    pyllbc_InitCommon();
    pyllbc_InitTestCase();
    pyllbc_InitCore();
    pyllbc_InitComm();
    pyllbc_InitApplication();

    /* Add all modules' objs/methods to python. */
    pyllbc_AddCommonObjs();
    pyllbc_AddTestCaseObjs();
    pyllbc_AddCoreObjs();
    pyllbc_AddCommObjs();
    pyllbc_AddApplicationObjs();

    pyllbc_AddCommonMethods();
    pyllbc_AddTestCaseMethods();
    pyllbc_AddCoreMethods();
    pyllbc_AddCommMethods();
    pyllbc_AddApplicationMethods();

    /* Buildup module. */
    pyllbc_TopModule->Build();

    /* Integrate script. */
    if (pyllbc_s_Scripts->Integrate() != LLBC_OK)
        return;
}
