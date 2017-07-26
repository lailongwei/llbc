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
#include "src/common/_Config.h"
#include "src/common/_MonkeyPatch.h"
#include "src/common/_VerInfo.h"
#include "src/core/file/_Dir.h"
#include "src/core/helper/_GUIDHelper.h"
#include "src/core/log/_Log.h"

static luaL_Reg lullbc_NativeMethods[] = {
    {"Dir_Create", _lullbc_Dir_Create},
    {"MonkeyPatchImpl_ToPointer", _lullbc_MonkeyPatchImpl_ToPointer},
    {"SetLibConfigToLua", _lullbc_SetLibConfigToLua},
    {"GetVersionInfo", _lullbc_GetVersionInfo},
    {"Dir_Remove", _lullbc_Dir_Remove},
    {"MonkeyPatchImpl_Table_Concat", _lullbc_MonkeyPatchImpl_Table_Concat},
    {"LogMsg", _lullbc_LogMsg},
    {"Cleanup", _lullbc_Cleanup},
    {"Startup", _lullbc_Startup},
    {"GUID_Generate", _lullbc_GUID_Generate},
    {"InitLog", _lullbc_InitLog},
    {"MonkeyPatch", _lullbc_MonkeyPatch},
    {"Dir_AbsPath", _lullbc_Dir_AbsPath},
    {"Dir_Join", _lullbc_Dir_Join},
    {"Dir_IsDir", _lullbc_Dir_IsDir},
    {NULL, NULL}
};
#endif // !__LULLBC_AUTOGEN_SRC__LULLBCMETHODS_H__

