/**
 * @file    AfterIncl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    205/09/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_AFTER_INCL_H__
#define __LLBC_COM_AFTER_INCL_H__

#include "llbc/common/PFConfig.h"
#include "llbc/common/Compiler.h"

// WIN32 platform MSVC compiler specific: pop warning settings.
#if LLBC_CUR_COMP == LLBC_COMP_MSVC
#pragma warning(pop)
#endif // LLBC_COMP_MSVC


#endif // !__LLBC_COM_AFTER_INCL_H__
