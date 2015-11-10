/**
 * @file    Application.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/09
 * @version 1.0
 *
 * @brief   The pyllbc communication top header file.
 */
#ifndef __PYLLBC_APP_APPLICATION_H__
#define __PYLLBC_APP_APPLICATION_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"
#include "pyllbc/comm/Comm.h"

/**
 * Initialize the pyllbc application module.
 */
LLBC_HIDDEN void pyllbc_InitApplication();

/**
 * Destroy the pyllbc application module.
 */
LLBC_HIDDEN void pyllbc_DestroyApplication();

/**
 * Add application module all methods to python.
 */
LLBC_HIDDEN void pyllbc_AddApplicationMethods();

/**
 * Add application module all objs to python.
 */
LLBC_HIDDEN void pyllbc_AddApplicationObjs();

#endif // !__PYLLBC_APP_APPLICATION_H__
