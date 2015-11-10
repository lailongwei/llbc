/**
 * @file    pyllbc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/j07/29
 * @version 1.0
 *
 * @brief   The llbc lib python lang wrapper header file.
 */
#ifndef __PYLLBC_H__
#define __PYLLBC_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/testcase/TestCase.h"
#include "pyllbc/core/Core.h"
#include "pyllbc/comm/Comm.h"
#include "pyllbc/application/Application.h"

/**
 * pyllbc library initialize function, call by python interpreter.
 */
#ifndef LLBC_DEBUG
PYLLBC_MOD_INIT_FUNC void initllbc();
#else // defined LLBC_DEBUG macro
PYLLBC_MOD_INIT_FUNC void initllbc_debug();
#endif // not define LLBC_DEBUG

#endif // !__PYLLBC_H__
