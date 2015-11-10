/**
 * @file    Core.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/09
 * @version 1.0
 *
 * @brief   The pyllbc core module top header file.
 */
#ifndef __PYLLBC_CORE_CORE_H__
#define __PYLLBC_CORE_CORE_H__

#include "pyllbc/common/Common.h"

/**
 * Initialize pyllbc core module.
 */
LLBC_HIDDEN void pyllbc_InitCore();

/**
 * Destroy pyllbc core module.
 */
LLBC_HIDDEN void pyllbc_DestroyCore();

/**
 * Add all core module methods to python.
 */
LLBC_HIDDEN void pyllbc_AddCoreMethods();

/**
 * Add all core module objs to python.
 */
LLBC_HIDDEN void pyllbc_AddCoreObjs();

#endif // !__PYLLBC_CORE_CORE_H__
