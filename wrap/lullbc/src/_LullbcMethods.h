/**
 *@file		_LullbcMethods.h
 *@author	Longwei Lai<lailongwei@126.com>
 *@date		<auto generate code, not tag date>
 *@version	1.0
 *
 *@brief	Auto generate by script tools, do not modify this file!!
 *@doc	
 */

#ifndef __LULLBC_AUTOGEN_SRC__LULLBCMETHODS_H__
#define __LULLBC_AUTOGEN_SRC__LULLBCMETHODS_H__

#include "lullbc/common/LibHeader.h"
#include "lullbc/common/Macro.h"
#include "src/_Lullbc.h"
#include "src/common/_VerInfo.h"
#include "src/common/_Config.h"
#include "src/core/log/_Log.h"

static luaL_Reg lullbc_NativeMethods[] = {
    {"Cleanup", _lullbc_Cleanup},
    {"Startup", _lullbc_Startup},
    {"SetLibConfigToLua", _lullbc_SetLibConfigToLua},
    {"GetVersionInfo", _lullbc_GetVersionInfo},
    {"InitLog", _lullbc_InitLog},
    {"LogMsg", _lullbc_LogMsg},
    {NULL, NULL}
};
#endif // !__LULLBC_AUTOGEN_SRC__LULLBCMETHODS_H__

