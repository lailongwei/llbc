/**
 * @file    Util_Text.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/02/09
 * @version 1.0
 *
 * @brief   LLBC library text processing APIs encapsulation.
 */
#ifndef __LLBC_CORE_UTILS_UTIL_TEXT_H__
#define __LLBC_CORE_UTILS_UTIL_TEXT_H__

#include "llbc/common/Common.h"

/**
 * Non-WIN32 platform compatible APIs. see MSDN.
 */
#if LLBC_TARGET_PLATFORM_NON_WIN32
LLBC_EXTERN LLBC_EXPORT int lstrlenA(LPCSTR lpString);
LLBC_EXTERN LLBC_EXPORT int lstrlenW(LPCWSTR lpString);

LLBC_EXTERN LLBC_EXPORT LPSTR lstrcatA(LPSTR lpString1, LPCSTR lpString2);
LLBC_EXTERN LLBC_EXPORT LPWSTR lstrcatW(LPWSTR lpString1, LPCWSTR lpString2);

LLBC_EXTERN LLBC_EXPORT int lstrcmpA(LPCSTR lpString1, LPCSTR lpString2);
LLBC_EXTERN LLBC_EXPORT int lstrcmpW(LPCWSTR lpString1, LPCWSTR lpString2);

LLBC_EXTERN LLBC_EXPORT int lstrcmpiA(LPCSTR lpString1, LPCSTR lpString2);
LLBC_EXTERN LLBC_EXPORT int lstrcmpiW(LPCWSTR lpString1, LPCWSTR lpString2);

LLBC_EXTERN LLBC_EXPORT LPSTR lstrcpyA(LPSTR lpString1, LPCSTR lpString2);
LLBC_EXTERN LLBC_EXPORT LPWSTR lstrcpyW(LPWSTR lpString1, LPCWSTR lpString2);

#ifdef LLBC_UNICODE
#define lstrlen lstrlenW
#define lstrcat lstrcatW
#define lstrcmp lstrcmpW
#define lstrcmpi lstrcmpiW
#define lstrcpy lstrcpyW
#else // LLBC_UNICODE
#define lstrlen lstrlenA
#define lstrcat lstrcatA
#define lstrcmp lstrcmpA
#define lstrcmpi lstrcmpiA
#define lstrcpy lstrcpyA
#endif // !LLBC_UNICODE
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

__LLBC_NS_BEGIN

/**
 * LLBC library style RAW string operation APIs.
 */
#define LLBC_StrLenA    ::lstrlenA
#define LLBC_StrLenW    ::lstrlenW

#define LLBC_StrCatA    ::lstrcatA
#define LLBC_StrCatW    ::lstrcatW

#define LLBC_StrCmpA    ::lstrcmpA
#define LLBC_StrCmpW    ::lstrcmpW

#define LLBC_StrCmpiA   ::lstrcmpiA
#define LLBC_StrCmpiW   ::lstrcmpiW

#define LLBC_StrCpyA    ::lstrcpyA
#define LLBC_StrCpyW    ::lstrcpyW

#ifdef LLBC_UNICODE
#define LLBC_StrLen     LLBC_StrLenW
#define LLBC_StrCat     LLBC_StrCatW
#define LLBC_StrCmp     LLBC_StrCmpW
#define LLBC_StrCmpi    LLBC_StrCmpiW
#define LLBC_StrCpy     LLBC_StrCmpW
#else   // LLBC_UNICODE
#define LLBC_StrLen     LLBC_StrLenA
#define LLBC_StrCat     LLBC_StrCatA
#define LLBC_StrCmp     LLBC_StrCmpA
#define LLBC_StrCmpi    LLBC_StrCmpiA
#define LLBC_StrCpy     LLBC_StrCpyA
#endif // !LLBC_UNICODE

/**
 * Split string using specific separator.
 * @param[in]  str            - the source string.
 * @param[in]  separator      - separator string.
 * @param[out] destStrList    - sestination string list.
 * @param[in]  justSplitFirst - split first flag, if true, when split one time, will stop.
 * @param[in]  escapeChar     - escape character, default is '\0'.
 */
LLBC_EXTERN LLBC_EXPORT void LLBC_SplitString(const LLBC_String &str,
                                         const LLBC_String &separator,
                                         std::vector<LLBC_String> &destStrList,
                                         bool justSplitFirst = false,
                                         char escapeChar = '\0');

/**
 * Filter out specific string in given string.
 * @param[in] str       - the sill filter out's string.
 * @param[in] filterStr - filter out string.
 * @return LLBC_String - the already filter out's string.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_FilterOutString(const LLBC_String &str, const LLBC_String &filterStr);

/**
 * Convert lower case character to upper case.
 * @param[in] str - will convert string.
 * @param[in] LLBC_String - the converted string.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_ToUpper(const char *str);

/**
 * Convert upper case character to lower case.
 * @param[in] str - will convert string.
 * @return LLBC_String - the converted string.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_ToLower(const char *str);

/**
 * @Trim string left & right ' ' or '\t' character.
 * @param[in] str     - the will trim's string.
 * @param[in] target  - the target character to be trimed.
 * @param[in] targets - A pointer to string containing the target characters to be trimed.
 * @return LLBC_String - the trimed string.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_Trim(const LLBC_String &str);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_Trim(const LLBC_String &str, char target);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_Trim(const LLBC_String &str, const char *targets);

/**
 * Trim string left ' ' or '\t' character.
 * @param[in] str     - the will trim's string.
 * @param[in] target  - the target character to be trimed.
 * @param[in] targets - A pointer to string containing the target characters to be trimed.
 * @return LLBC_String - the trimed string.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_TrimLeft(const LLBC_String &str);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_TrimLeft(const LLBC_String &str, char target);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_TrimLeft(const LLBC_String &str, const char *targets);

/**
 * Trim string right ' ' or '\t' character.
 * @param[in] str     - the will trim's string.
 * @param[in] target  - the target character to be trimed.
 * @param[in] targets - A pointer to string containing the target characters to be trimed.
 * @return LLBC_String - the trimed string.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_TrimRight(const LLBC_String &str);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_TrimRight(const LLBC_String &str, char target);
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_TrimRight(const LLBC_String &str, const char *targets);

/**
 * Get given path's directory name.
 * @param[in] path - given path.
 * @return LLBC_String - directory part name.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_DirName(const LLBC_String &path);

/**
 * Get given path's file name.
 * @param[in] path         - given path.
 * @param[in] incExtension - include extension name.
 * @return LLBC_String - file name part name.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_BaseName(const LLBC_String &path, bool incExtension = true);

/**
 * Get given path's file extension name, not include '.' character.
 * @param[in] path - given path.
 * @return LLBC_String - file extension name.
 */
LLBC_EXTERN LLBC_EXPORT LLBC_String LLBC_ExtensionName(const LLBC_String &path);

/**
 * Convert string to number(signed/unsigned)(int, long, long long, pointer, double) type.
 * @param[in] str - the string value.
 * @return number - the converted vavlue.
 */
LLBC_EXTERN LLBC_EXPORT sint32 LLBC_Str2Int32(const char *str);
LLBC_EXTERN LLBC_EXPORT uint32 LLBC_Str2UInt32(const char *str);
LLBC_EXTERN LLBC_EXPORT long LLBC_Str2Long(const char *str);
LLBC_EXTERN LLBC_EXPORT ulong LLBC_Str2ULong(const char *str);
LLBC_EXTERN LLBC_EXPORT sint64 LLBC_Str2Int64(const char *str);
LLBC_EXTERN LLBC_EXPORT uint64 LLBC_Str2UInt64(const char *str);
LLBC_EXTERN LLBC_EXPORT void *LLBC_Str2Ptr(const char *str);
LLBC_EXTERN LLBC_EXPORT double LLBC_Str2Double(const char *str);

/**
 * Convert number(signed/unsigned) type to string format.
 * @param[in] val   - number value.
 * @param[in] radix - base of value, must be in range[2, 36](specially, void * type 
 *                    instantiate function just support 10, 16 base value).
 * @return LLBC_String - the converted string.
 */
template <typename T>
LLBC_String LLBC_Num2Str(T val, int radix = 10);

__LLBC_NS_END

#include "llbc/core/utils/Util_TextImpl.h"

#endif // __LLBC_CORE_UTILS_UTIL_TEXT_H__
