// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
    if (LLBC_Startup() != LLBC_OK &&
        LLBC_GetLastError() != LLBC_ERROR_REENTRY)
    {
        LLBC_String errStr;
        errStr.format("Initialize llbc library failed, err: %s", LLBC_FormatLastError());

        PyErr_SetString(PyExc_Exception, errStr.c_str());

        return;
    }

    /* Init pyllbc modules. */
    pyllbc_InitCommon();
    pyllbc_InitTestCase();
    pyllbc_InitCore();
    pyllbc_InitComm();
    pyllbc_InitApp();

    /* Add all modules' objs/methods to python. */
    pyllbc_AddCommonObjs();
    pyllbc_AddTestCaseObjs();
    pyllbc_AddCoreObjs();
    pyllbc_AddCommObjs();
    pyllbc_AddAppObjs();

    pyllbc_AddCommonMethods();
    pyllbc_AddTestCaseMethods();
    pyllbc_AddCoreMethods();
    pyllbc_AddCommMethods();
    pyllbc_AddAppMethods();

    /* Buildup module. */
    pyllbc_TopModule->Build();

    /* Integrate script. */
    if (pyllbc_s_Scripts->Integrate() != LLBC_OK)
        return;
}
