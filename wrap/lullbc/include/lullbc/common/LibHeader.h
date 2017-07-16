/**
 * @file    LibHeader.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017-07-06
 * @version 1.0
 *
 * @brief   
 */
#ifndef __LULLBC_COM_LIB_HEADER_H__
#define __LULLBC_COM_LIB_HEADER_H__

/* Include lua header files */
#ifdef __cplusplus
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#endif // __cplusplus

/* Include lua ext file */
#include "lullbc/common/LuaExt.h"

/* Include llbc library header file */
#include "llbc.h"
using namespace llbc;

#endif // !__LULLBC_COM_LIB_HEADER_H__
