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
#include "src/core/thread/_Thread.h"
#include "src/core/timer/_Timer.h"
#include "src/core/timer/_TimerScheduler.h"
#include "src/core/utils/_Util_Misc.h"
#include "src/core/utils/_Util_String.h"
#include "src/core/utils/_Util_Table.h"

static luaL_Reg lullbc_NativeMethods[] = {
    {"Dir_Remove", _lullbc_Dir_Remove},
    {"Util_Misc_ToPointer", _lullbc_Util_Misc_ToPointer},
    {"TimerScheduler_GetTimerCount", _lullbc_TimerScheduler_GetTimerCount},
    {"InitLog", _lullbc_InitLog},
    {"GetVersionInfo", _lullbc_GetVersionInfo},
    {"Timer_GetPeriod", _lullbc_Timer_GetPeriod},
    {"Cleanup", _lullbc_Cleanup},
    {"LogMsg", _lullbc_LogMsg},
    {"Timer_GetDueTime", _lullbc_Timer_GetDueTime},
    {"Timer_GetTimerId", _lullbc_Timer_GetTimerId},
    {"MonkeyPatch", _lullbc_MonkeyPatch},
    {"Dir_AbsPath", _lullbc_Dir_AbsPath},
    {"Dir_Create", _lullbc_Dir_Create},
    {"TimerScheduler_SetTimerErrorHandler", _lullbc_TimerScheduler_SetTimerErrorHandler},
    {"Util_String_HashString", _lullbc_Util_String_HashString},
    {"SetLibConfigToLua", _lullbc_SetLibConfigToLua},
    {"TimerScheduler_SetEnabled", _lullbc_TimerScheduler_SetEnabled},
    {"TimerScheduler_IsEnabled", _lullbc_TimerScheduler_IsEnabled},
    {"Timer_IsScheduling", _lullbc_Timer_IsScheduling},
    {"TimerScheduler_Update", _lullbc_TimerScheduler_Update},
    {"Startup", _lullbc_Startup},
    {"Util_Table_Concat", _lullbc_Util_Table_Concat},
    {"Dir_IsDir", _lullbc_Dir_IsDir},
    {"Timer_New", _lullbc_Timer_New},
    {"GUID_Generate", _lullbc_GUID_Generate},
    {"Dir_Join", _lullbc_Dir_Join},
    {"Thread_Sleep", _lullbc_Thread_Sleep},
    {"Timer_Schedule", _lullbc_Timer_Schedule},
    {NULL, NULL}
};
#endif // !__LULLBC_AUTOGEN_SRC__LULLBCMETHODS_H__

