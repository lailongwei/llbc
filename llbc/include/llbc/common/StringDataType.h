/**
 * @file    StringDataType.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_STRING_DATA_TYPE_H__
#define __LLBC_COM_STRING_DATA_TYPE_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"
#include "llbc/common/BasicString.h"

__LLBC_NS_BEGIN

typedef LLBC_BasicString<char> LLBC_String;
typedef LLBC_BasicString<wchar> LLBC_WString;

typedef std::vector<LLBC_BasicString<char> > LLBC_Strings;
typedef std::vector<LLBC_BasicString<wchar> > LLBC_WStrings;

#ifdef LLBC_UNICODE
 typedef LLBC_WString LLBC_TString;
 typedef LLBC_WStrings LLBC_TStrings;
#else // !LLBC_UNICODE
 typedef LLBC_String LLBC_TString;
 typedef LLBC_Strings LLBC_TStrings;
#endif // LLBC_UNICODE

__LLBC_NS_END

#endif // !__LLBC_COM_STRING_DATA_TYPE_H__
