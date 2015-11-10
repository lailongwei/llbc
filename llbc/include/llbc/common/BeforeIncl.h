/**
 * @file    BeforeIncl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/09/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_BEFORE_INCL_H__
#define __LLBC_COM_BEFORE_INCL_H__

#include "llbc/common/PFConfig.h"
#include "llbc/common/Compiler.h"

// WIN32 platform MSVC compiler specific: push old warning settings, and set library warning settings.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC
#pragma warning(push, 3) // push project warning settings, and set library warning level to 3.
#pragma warning(disable: 4251) // disable the 4251 warning, we ignore it, about C4251, see MSDN.
#endif // LLBC_COMP_MSVC

#endif // !__LLBC_COM_BEFORE_INCL_H__
