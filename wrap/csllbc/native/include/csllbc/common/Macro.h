/**
 * @file    Macro.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2016/01/25
 * @version 1.0
 *
 * @brief
 */
#ifndef __CSLLBC_COM_MACRO_H__
#define __CSLLBC_COM_MACRO_H__

#include "llbc/common/Macro.h"

// the hidden/export/exporting macro define, same with llbc core library.
#ifndef CSLLBC_HIDDEN
 #define CSLLBC_HIDDEN LLBC_HIDDEN
#endif

#ifndef CSLLBC_EXPORT
 #define CSLLBC_EXPORT LLBC_EXPORT
#endif

#ifndef CSLLBC_EXPORTING
 #define CSLLBC_EXPORTING LLBC_EXPORTING
#endif

#endif // !__CSLLBC_COM_MACRO_H__
