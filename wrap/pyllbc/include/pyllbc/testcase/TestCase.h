/**
 * @file    TestCase.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_TESTCASE_TESTCASE_H__
#define __PYLLBC_TESTCASE_TESTCASE_H__

#include "pyllbc/common/Common.h"

/**
 * Initialize pyllbc testcase module.
 */
LLBC_HIDDEN void pyllbc_InitTestCase();

/**
 * Destroy pyllbc testcase module.
 */
LLBC_HIDDEN void pyllbc_DestroyTestCase();

/**
 * Add testcase module all methods to python.
 */
LLBC_HIDDEN void pyllbc_AddTestCaseMethods();

/**
 * Add testcase module all objects to python.
 */
LLBC_HIDDEN void pyllbc_AddTestCaseObjs();

#endif // !__PYLLBC_TESTCASE_TESTCASE_H__
