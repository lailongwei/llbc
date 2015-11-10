/**
 * @file    Comm.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/08/09
 * @version 1.0
 *
 * @brief   The pyllbc communication module top header file.
 */
#ifndef __PYLLBC_COMM_COMM_H__
#define __PYLLBC_COMM_COMM_H__

#include "pyllbc/common/Common.h"
#include "pyllbc/core/Core.h"

/**
 * Initialize the communication module.
 */
LLBC_HIDDEN void pyllbc_InitComm();

/**
 * Destroy the communication module.
 */
LLBC_HIDDEN void pyllbc_DestroyComm();

/**
 * Add all communication module methods to python.
 */
LLBC_HIDDEN void pyllbc_AddCommMethods();

/**
 * Ad all communication objs to python.
 */
LLBC_HIDDEN void pyllbc_AddCommObjs();

#endif // !__PYLLBC_COMM_COMM_H__
