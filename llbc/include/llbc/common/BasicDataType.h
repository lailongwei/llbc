// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"
#include "llbc/common/OSHeader.h"
#include "llbc/common/Config.h"

__LLBC_NS_BEGIN

// basic number type define.
#if LLBC_TARGET_PLATFORM_NON_WIN32
 typedef char sint8;
 typedef short sint16;
 typedef int sint32;
 typedef long long sint64;
 typedef unsigned char uint8;
 typedef unsigned char byte;
 typedef unsigned short uint16;
 typedef unsigned int uint32;
 typedef unsigned long ulong;
 typedef unsigned long long uint64;
 typedef long double ldouble;
 typedef unsigned short wchar;
#else // LLBC_TARGET_PLATFORM_WIN32
 typedef __int8 sint8;
 typedef __int16 sint16;
 typedef __int32 sint32;
 typedef __int64 sint64;
 typedef unsigned __int8 uint8;
 typedef unsigned __int16 uint16;
 typedef unsigned __int32 uint32;
 typedef unsigned long ulong;
 typedef unsigned __int64 uint64;
 typedef long double ldouble;
 typedef __wchar_t wchar;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

typedef sint8 * sint8_ptr;
typedef sint16 * sint16_ptr;
typedef sint32 * sint32_ptr;
typedef sint64 * sint64_ptr;
typedef uint8 * uint8_ptr;
typedef uint16 * uint16_ptr;
typedef uint32 * uint32_ptr;
typedef uint64 * uint64_ptr;
typedef long * long_ptr;
typedef ulong * ulong_ptr;
typedef float * float_ptr;
typedef double * double_ptr;
typedef ldouble *ldouble_ptr;
typedef wchar * wchar_ptr;
typedef void * void_ptr;

// TimeStruct define.
extern "C" {
struct LLBC_TimeStruct
{
    int tm_sec;     /* seconds after the minute - [0,59] */
    int tm_min;     /* minutes after the hour - [0,59] */
    int tm_hour;    /* hours since midnight - [0,23] */
    int tm_mday;    /* day of the month - [1,31] */
    int tm_mon;     /* months since January - [0,11] */
    int tm_year;    /* years since 1900 */
    int tm_wday;    /* days since Sunday - [0,6] */
    int tm_yday;    /* days since January 1 - [0,365] */
    int tm_isdst;   /* daylight savings time flag */
    int tm_gmtoff;  /* gmt offset, in secods. */
};
}
// LLBC_UNICODE macro define.
#if (LLBC_TARGET_PLATFORM_WIN32) && defined(_MSC_VER)
 #ifdef _UNICODE
  #ifndef LLBC_UNICODE
   #define LLBC_UNICODE
  #endif // LLBC_UNICODE
 #else // LLBC_TARGET_PLATFORM_NON_WIN32 || !defined(_MSC_VER)
  #ifdef LLBC_UNICODE
   #error "llbc library - MS unicode is set to 'using multi-character', but LLBC_UNICODE macro was defined!"
  #endif // LLBC_UNICODE
 #endif // _UNICODE
#endif // LLBC_PLATFORM_WIN32 && _MSC_VER

// Wide character set macro.
#define LLBC_WTEXT(x)   L##x
#define __WTEXT(x)      LLBC_WTEXT(x)
#define WTEXT(x)        __WTEXT(x)
#define __W(x)          LLBC_WTEXT(x)
#define _W(x)           __W(x)

// tchar datatype define.
#ifdef LLBC_UNICODE
 typedef wchar tchar;
 #define LLBC_TEXT(x)    L##x
#else
 typedef char tchar;
 #define LLBC_TEXT(x)    x
#endif // LLBC_UNICODE

__LLBC_NS_END

// Like WIN32, we define Some char/wchar/tchar pointer/const pointer.
#if LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_CORE_UTILS_DEF_WIN32_STYLED_STR_DATATYPES
 typedef char * LPSTR;
 typedef LLBC_NS wchar * LPWSTR;
 typedef const char * LPCSTR;
 typedef const LLBC_NS wchar * LPCWSTR;

 #ifdef LLBC_UNICODE
  #define LPTSTR  LPWSTR
  #define LPCTSTR LPCWSTR
 #else   // !defined(LLBC_UNICODE)
  #define LPTSTR  LPSTR
  #define LPCTSTR LPCSTR
 #endif // !LLBC_UNICODE
#endif // LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_CORE_UTILS_DEF_WIN32_STYLED_STR_DATATYPES

// Like WIN32, we define the TCHAR data type.
#if LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_CORE_UTILS_DEF_WIN32_STYLED_STR_DATATYPES
 typedef LLBC_NS tchar TCHAR;
#endif // LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_CORE_UTILS_DEF_WIN32_STYLED_STR_DATATYPES

__LLBC_NS_BEGIN

// Like WIN32, we define the _T, __TEXT, TEXT macro to simple use LLBC_TEXT() macro.
#if LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_CORE_UTILS_DEF_WIN32_STYLED_STR_DATATYPES
 #define __TEXT()       LLBC_TEXT(x)
 #define TEXT()         __TEXT(x)
 #define __T(x)         LLBC_TEXT(x)
 #define _T(x)          __T(x)
#endif // LLBC_TARGET_PLATFORM_NON_WIN32 && LLBC_CFG_CORE_UTILS_DEF_WIN32_STYLED_STR_DATATYPES

// handle datatype define.
typedef int LLBC_Handle;
#define LLBC_INVALID_HANDLE                 ((LLBC_NS LLBC_Handle)(-1))

// thread datatype define.
#if LLBC_TARGET_PLATFORM_LINUX
 typedef pthread_t LLBC_NativeThreadHandle;
 #define LLBC_INVALID_NATIVE_THREAD_HANDLE   ((pthread_t)(nullptr))
 typedef int LLBC_ThreadId;
 #define LLBC_INVALID_NATIVE_THREAD_ID       0
 typedef pthread_mutex_t LLBC_MutexHandle;
 typedef pthread_spinlock_t LLBC_SpinLockHandle;
 typedef pthread_key_t LLBC_TlsHandle;
 #define LLBC_INVALID_TLS_HANDLE             ((pthread_key_t)(-1))
#elif LLBC_TARGET_PLATFORM_WIN32
 typedef HANDLE LLBC_NativeThreadHandle;
 #define LLBC_INVALID_NATIVE_THREAD_HANDLE INVALID_HANDLE_VALUE
 typedef int LLBC_ThreadId;
 #define LLBC_INVALID_NATIVE_THREAD_ID       0
 typedef CRITICAL_SECTION LLBC_MutexHandle;
 typedef CRITICAL_SECTION LLBC_SpinLockHandle;
 typedef DWORD LLBC_TlsHandle;
 #define LLBC_INVALID_TLS_HANDLE             ((DWORD)(-1))
#elif LLBC_TARGET_PLATFORM_IPHONE
 typedef pthread_t LLBC_NativeThreadHandle;
 #define LLBC_INVALID_NATIVE_THREAD_HANDLE   ((pthread_t)(nullptr))
 typedef pthread_id_np_t LLBC_ThreadId;
 #define LLBC_INVALID_NATIVE_THREAD_ID       0
 typedef pthread_mutex_t LLBC_MutexHandle;
 typedef pthread_mutex_t LLBC_SpinLockHandle;
 typedef pthread_key_t LLBC_TlsHandle;
 #define LLBC_INVALID_TLS_HANDLE             ((pthread_key_t)(-1))
#elif LLBC_TARGET_PLATFORM_MAC
 typedef pthread_t LLBC_NativeThreadHandle;
 #define LLBC_INVALID_NATIVE_THREAD_HANDLE   ((pthread_t)(nullptr))
 typedef int LLBC_ThreadId;
 #define LLBC_INVALID_NATIVE_THREAD_ID       0
 typedef pthread_mutex_t LLBC_MutexHandle;
 typedef pthread_mutex_t LLBC_SpinLockHandle;
 typedef pthread_key_t LLBC_TlsHandle;
 #define LLBC_INVALID_TLS_HANDLE             ((pthread_key_t)(-1))
#elif LLBC_TARGET_PLATFORM_ANDROID
 typedef pthread_t LLBC_NativeThreadHandle;
 #define LLBC_INVALID_NATIVE_THREAD_HANDLE   ((pthread_t)(nullptr))
 typedef int LLBC_ThreadId;
 #define LLBC_INVALID_NATIVE_THREAD_ID       0
 typedef pthread_mutex_t LLBC_MutexHandle;
 typedef pthread_spinlock_t LLBC_SpinLockHandle;
 typedef pthread_key_t LLBC_TlsHandle;
 #define LLBC_INVALID_TLS_HANDLE             ((pthread_key_t)(-1))
#endif // LLBC_TARGET_PLATFORM_LINUX

// Thread about data type define.
typedef void LLBC_ThreadRtn;
typedef void * LLBC_ThreadArg;
typedef LLBC_ThreadRtn (*LLBC_ThreadProc)(LLBC_ThreadArg);

// file data type define.
typedef FILE * LLBC_FileHandle;
#define LLBC_INVALID_FILE_HANDLE nullptr

// library data type define.
#if LLBC_TARGET_PLATFORM_NON_WIN32
 typedef void * LLBC_LibraryHandle;
 typedef int (*LLBC_LibraryFun)(void);
#else
 typedef HMODULE LLBC_LibraryHandle;
 typedef int (__cdecl *LLBC_LibraryFun)(void);
#endif
#define LLBC_INVALID_LIBRARY_HANDLE nullptr

// timer data type define.
typedef uint64 LLBC_TimerId;
#define LLBC_INVALID_TIMER_ID    (0)
#if LLBC_TARGET_PLATFORM_WIN32
extern "C"
{
    // For compatible with Linux like system, 
    // we define timespec structure in WIN32 platform.
    typedef struct _timespec 
    {
        time_t tv_sec; /* seconds */
        long tv_nsec;  /* nanoseconds */
    } timespec;
}
#endif

// guid data type define.
#if LLBC_TARGET_PLATFORM_NON_WIN32
struct LLBC_GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
};
#else
typedef GUID LLBC_GUID;
#endif

typedef LLBC_GUID & LLBC_GUIDRef;
typedef const LLBC_GUID & LLBC_GUIDCRef;

// Bundle data type define.
#if LLBC_TARGET_PLATFORM_IPHONE
 #define LLBC_BundleHandle void *
 #define LLBC_INVALID_BUNDLE_HANDLE nullptr
#else
 #define LLBC_BundleHandle void *
 #define LLBC_INVALID_BUNDLE_HANDLE nullptr
#endif

// Session Ids data type define.
typedef std::vector<int> LLBC_SessionIdList;
typedef LLBC_SessionIdList::iterator LLBC_SessionIdListIter;
typedef LLBC_SessionIdList::const_iterator LLBC_SessionIdListCIter;

typedef std::set<int> LLBC_SessionIdSet;
typedef LLBC_SessionIdSet::iterator LLBC_SessionIdSetIter;
typedef LLBC_SessionIdSet::const_iterator LLBC_SessionIdSetCIter;

// Component dynamic create function define
// Note: the return pointer must be LLBC_Component(or its subclass) class instance.
typedef void *(*LLBC_CompDynamicCreateFunc)();

__LLBC_NS_END
