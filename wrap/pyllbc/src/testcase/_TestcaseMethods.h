/**
 *@file		_TestcaseMethods.h
 *@author	Longwei Lai<lailongwei@126.com>
 *@date		<auto generate code, not tag date>
 *@version	1.0
 *
 *@brief	Auto generate by script tools, do not modify this file!!
 *@doc	
 */

#ifndef __PYLLBC_AUTOGEN_TESTCASE__TESTCASEMETHODS_H__
#define __PYLLBC_AUTOGEN_TESTCASE__TESTCASEMETHODS_H__

#include "pyllbc/common/LibHeader.h"
#include "pyllbc/common/Macro.h"
#include "src/testcase/_TestCase.h"

class pyllbc_TestcaseMethods
{
public:
    pyllbc_TestcaseMethods()
    {
        DelTestCase.ml_name = "DelTestCase";
        DelTestCase.ml_meth = (PyCFunction)_pyllbc_DelTestCase;
        DelTestCase.ml_flags = METH_VARARGS;
        DelTestCase.ml_doc = "pyllbc library method/function";
        RunTestCase.ml_name = "RunTestCase";
        RunTestCase.ml_meth = (PyCFunction)_pyllbc_RunTestCase;
        RunTestCase.ml_flags = METH_VARARGS;
        RunTestCase.ml_doc = "pyllbc library method/function";
        GetTestCaseName.ml_name = "GetTestCaseName";
        GetTestCaseName.ml_meth = (PyCFunction)_pyllbc_GetTestCaseName;
        GetTestCaseName.ml_flags = METH_VARARGS;
        GetTestCaseName.ml_doc = "pyllbc library method/function";
        NewTestCase.ml_name = "NewTestCase";
        NewTestCase.ml_meth = (PyCFunction)_pyllbc_NewTestCase;
        NewTestCase.ml_flags = METH_VARARGS;
        NewTestCase.ml_doc = "pyllbc library method/function";
        SetTestCaseName.ml_name = "SetTestCaseName";
        SetTestCaseName.ml_meth = (PyCFunction)_pyllbc_SetTestCaseName;
        SetTestCaseName.ml_flags = METH_VARARGS;
        SetTestCaseName.ml_doc = "pyllbc library method/function";
    }

public:
    ::PyMethodDef DelTestCase;
    ::PyMethodDef RunTestCase;
    ::PyMethodDef GetTestCaseName;
    ::PyMethodDef NewTestCase;
    ::PyMethodDef SetTestCaseName;
};

#define pyllbc_s_TestcaseMethods LLBC_Singleton< ::pyllbc_TestcaseMethods, LLBC_DummyLock>::Instance()

#endif // !__PYLLBC_AUTOGEN_TESTCASE__TESTCASEMETHODS_H__

