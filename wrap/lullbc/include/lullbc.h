/**
 * @file    lullbc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2017/07/15
 * @version 1.0
 *
 * @brief
 */
#ifndef __LULLBC_LULLBC_H__
#define __LULLBC_LULLBC_H__

#include "lullbc/common/Common.h"
#include "lullbc/core/Core.h"

/**
 * lullbc entry function, call by lua program.
 */
LULLBC_EXTERN_C LULLBC_EXPORT int lullbc_entry(lua_State *l);

#endif // !__LULLBC_LULLBC_H__