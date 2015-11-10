/**
 * @file    LibHeader.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2104/07/30
 * @version 1.0
 *
 * @brief
 */
#ifndef __PYLLBC_COM_LIB_HEADER_H__
#define __PYLLBC_COM_LIB_HEADER_H__

/* Include python header file. */
#include "Python.h"
#include "frameobject.h"

/* Include llbc lib header file. */
#include "llbc.h"
using namespace llbc;

/* Full stack option check. */
#if LLBC_CFG_COMM_USE_FULL_STACK
 #error "pyllbc library could not use LLBC_CFG_COMM_USE_FULL_STACK option is true's llbc library to compile! please set this option to '0'"
#endif


#endif // !__PYLLBC_COM_LIB_HEADER_H__

