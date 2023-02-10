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


#include "llbc/common/Export.h"

#include "llbc/common/OSHeader.h"
#include "llbc/common/Define.h"
#include "llbc/common/Errno.h"
#include "llbc/common/LibTlsInl.h"
#include "llbc/common/StringDataType.h"

#include "llbc/common/Errors.h"

__LLBC_INTERNAL_NS_BEGIN
static const char __g_invalidErrDesc[__LLBC_ERROR_DESC_SIZE] = {0};
__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

static const char *__g_errDesc[__LLBC_ERROR_SENTINEL] =
{
    // Successfully.
    "successfully",             // 0x0000

    // C lib error.
    "c lib error(%d):%s",       // 0x0001
    // OS API error.
    "OS API error(%d):%s",      // 0x0002
    // Net lib error.
    "Net error(WSA)(%d):%s",    // 0x0003
    // gai error(getaddrinfo).
    "gai error(%d):%s",         // 0x0004

    // Argument error.
    "argument error",           // 0x0005
    // not implement.
    "not implement",            // 0x0006
    // range error.
    "range error",              // 0x0007
    // exist.
    "exist",                    // 0x0008
    // not found.
    "not found",                // 0x0009
    // end.
    "end",                      // 0x000a
    // limit.
    "limit",                    // 0x000b
    // not init.
    "not init",                 // 0x000c
    // not open.
    "not open",                 // 0x000d
    // initialized.
    "initialized",              // 0x000e
    // opened.
    "opened",                   // 0x000f
    // permission denied.
    "permission denied",        // 0x0010

    // truncated.
    "truncated",                // 0x0011
    // invalidate.
    "invalidate",               // 0x0012
    // format error.
    "format error",             // 0x0013
    // timeouted.
    "timeouted",                // 0x0014
    // busy.
    "busy",                     // 0x0015
    // reentry.
    "reentry",                  // 0x0016
    // no such memory/data/res.
    "no such memory/data/res",  // 0x0017
    // would block.
    "would block",              // 0x0018
    // again.
    "again",                    // 0x0019
    // pending.
    "pending",                  // 0x001a
    // illegal.
    "illegal",                  // 0x001b
    // access deny.
    "access deny",              // 0x001c
    // repeat.
    "repeat",                   // 0x001d
    // encode error.
    "encode error",             // 0x001e
    // decode error.
    "decode error",             // 0x001f
    // compress error.
    "compress error",           // 0x0020
    // decompress error.
    "decompress error",         // 0x0021
    // pack error.
    "pack error",               // 0x0022
    // unpack error.
    "unpack error",             // 0x0023
    // not allow.
    "not allow",                // 0x0024
    // initialize comp failed.
    "initialize comp failed",   // 0x0025
    // start comp failed.
    "start comp failed",        // 0x0026
    // WSA specific: WSASYSNOTREADY
    "underlying network subsystem is not ready for network communication", // 0x0027
    // WSA specific: WSAVERNOTSUPPORTED
    "the version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation", // 0x0028
    // WSA specific: WSAEINPROGRESS
    "a blocking windows Sockets 1.1 operations in progress", // 0x0029
    // WSA specific: WSAEPROCLIM
    "limit on the number of tasks supported by the Windows Sockets implementation has been reached", // 0x002a
    // the specific socket is listen socket.
    "the specific socket is listen socket", // 0x002b
    // object is not pool object.
    "object is not pool object",// 0x002c
    // session send buffer limit.
    "session send buffer limit",// 0x002d
    // not support.
    "not support",              // 0x002e
    // cancelled.
    "cancelled",                // 0x002f
};

static std::map<int, LLBC_String> __g_customErrDesc;
static const char *__g_unknownCustomErrDesc = "Unknown custom error";

LLBC_MutexHandle __g_customErrLock;

static void __LLBC_LockCustomErr()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_testcancel();
    pthread_mutex_lock(&__g_customErrLock);
#else // LLBC_TARGET_PLATFORM_WIN32
    ::EnterCriticalSection(&__g_customErrLock);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

static void __LLBC_UnlockCustomErr()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_unlock(&__g_customErrLock);
    pthread_testcancel();
#else // LLBC_TARGET_PLATFORM_WIN32
    ::LeaveCriticalSection(&__g_customErrLock);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

int LLBC_GetLastError()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (LIKELY(libTls))
        return libTls->commonTls.errNo;

    return LLBC_ERROR_SUCCESS;
}

void LLBC_SetLastError(int no, const char *customErrStr)
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
        return;

    // Set errNo.
    libTls->commonTls.errNo = no;

    // Set subErrNo.
    if (LLBC_ERROR_TYPE_IS_CLIB(no))
    {
        libTls->commonTls.subErrNo = errno;
    }
#if LLBC_TARGET_PLATFORM_WIN32
    else if (LLBC_ERROR_TYPE_IS_OSAPI(no))
    {
        libTls->commonTls.subErrNo = ::GetLastError();
    }
    else if (LLBC_ERROR_TYPE_IS_NETAPI(no))
    {
        libTls->commonTls.subErrNo = ::WSAGetLastError();
    }
#endif
    else // Library error or Custom error.
    {
        // Set custom error string(only set when error type is LIBRARY or is CUSTOM errno).
        size_t customErrStrLen;
        if (customErrStr &&
            (LLBC_ERROR_TYPE_IS_LIBRARY(no) || (no & LLBC_ERROR_MASK_CUSTOM) != 0) &&
            (customErrStrLen = strlen(customErrStr)) > 0)
        {
            const size_t copyLen = MIN(customErrStrLen, sizeof(libTls->commonTls.customErrDesc) - 1);
            memcpy(libTls->commonTls.customErrDesc, customErrStr, copyLen);
            libTls->commonTls.customErrDesc[copyLen] = '\0';
        }
        else
        {
            libTls->commonTls.customErrDesc[0] = '\0';
        }
    }
}

const char *LLBC_StrError(int no)
{
    return LLBC_StrErrorEx(no, LLBC_GetSubErrorNo());
}

const char *LLBC_StrErrorEx(int no, int subErrno)
{
    //！First, get lib tls.
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (UNLIKELY(!libTls))
        return LLBC_INTERNAL_NS __g_invalidErrDesc;

    // Process custom type error.
    if (UNLIKELY(LLBC_GetErrnoCustomPart(no) == LLBC_ERROR_CUSTOM))
    {
        if (libTls->commonTls.customErrDesc[0] != '\0')
            return libTls->commonTls.customErrDesc;

        __LLBC_LockCustomErr();

        typedef std::map<int, LLBC_String>::iterator _Iter;
        _Iter it = __g_customErrDesc.find(no);
        const bool found = it != __g_customErrDesc.end();

        const char *errDesc =
            found ? it->second.c_str() : __g_unknownCustomErrDesc;

        __LLBC_UnlockCustomErr();

        return errDesc;
    }

    uint32 noPart = LLBC_GetErrnoNoPart(no);
    if (LLBC_ERROR_TYPE_IS_CLIB(no))
    {
        snprintf(libTls->commonTls.errDesc,
                 __LLBC_ERROR_DESC_SIZE,
                 __g_errDesc[noPart],
                 subErrno,
                 #if LLBC_TARGET_PLATFORM_WIN32
                 strerror_s(libTls->commonTls.clibErrFmtBuf,
                            __LLBC_CLIB_ERROR_FORMAT_BUF_SIZE,
                            subErrno) == 0 ? 
                                libTls->commonTls.clibErrFmtBuf : "Unknown <errno>");
                 #else // Non-Win32
                 strerror(subErrno));
                 #endif // Win32
        return libTls->commonTls.errDesc;
    }
#if LLBC_TARGET_PLATFORM_WIN32
    else if (LLBC_ERROR_TYPE_IS_OSAPI(no) || LLBC_ERROR_TYPE_IS_NETAPI(no) || LLBC_ERROR_TYPE_IS_GAI(no))
    {
        HLOCAL hLocal = nullptr;
        const DWORD sysLocale = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        // const DWORD sysLocale = MAKELANGID(LANG_SYSTEM_DEFAULT, SUBLANG_SYS_DEFAULT);
        ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
                             FORMAT_MESSAGE_ALLOCATE_BUFFER |
                             FORMAT_MESSAGE_IGNORE_INSERTS,
                         nullptr,
                         subErrno,
                         sysLocale,
                         (PSTR)&hLocal,
                         0,
                         nullptr);
        if (!hLocal)
        {
            HMODULE netDll =
                LoadLibraryExA("netmsg.dll", nullptr, DONT_RESOLVE_DLL_REFERENCES);
            if (netDll != nullptr)
            {
                ::FormatMessageA(FORMAT_MESSAGE_FROM_HMODULE |
                                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                    FORMAT_MESSAGE_IGNORE_INSERTS,
                                 netDll,
                                 subErrno,
                                 sysLocale,
                                 (PSTR)&hLocal,
                                 0,
                                 nullptr);

                ::FreeLibrary(netDll);
            }
        }

        if (hLocal != nullptr)
        {
            PSTR sysErr = (PSTR)::LocalLock(hLocal);

            bool hasCRLF = false;
            const size_t sysErrLen = strlen(sysErr);
            if (sysErrLen >= 2)
                if (sysErr[sysErrLen - 2] == LLBC_CR_A &&
                    sysErr[sysErrLen - 1] == LLBC_LF_A)
                        hasCRLF = true;

            if (hasCRLF)
                sysErr[sysErrLen - 2] = '\0';

            ::LocalUnlock(hLocal);

            sprintf_s(libTls->commonTls.errDesc,
                  __LLBC_ERROR_DESC_SIZE, __g_errDesc[noPart], subErrno, sysErr);

            ::LocalFree(hLocal);
        }
        else
        {
            char unknownErrDesc[64] = {0};
            sprintf_s(unknownErrDesc,
                sizeof(unknownErrDesc), "Unknown error, error code: %d", subErrno);
            sprintf_s(libTls->commonTls.errDesc,
                __LLBC_ERROR_DESC_SIZE, __g_errDesc[noPart], unknownErrDesc);
        }

        return libTls->commonTls.errDesc;
    }
#endif // LLBC_TARGET_PLATFORM_WIN32
#if LLBC_TARGET_PLATFORM_NON_WIN32
    else if (LLBC_ERROR_TYPE_IS_GAI(no))
    {
        snprintf(libTls->commonTls.errDesc,
                 __LLBC_ERROR_DESC_SIZE,
                 __g_errDesc[noPart],
                 subErrno,
                 gai_strerror(subErrno));
        return libTls->commonTls.errDesc;
    }
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
    else if (noPart == __LLBC_ERROR_UNKNOWN ||
        noPart >= __LLBC_ERROR_SENTINEL)
    {
#if LLBC_TARGET_PLATFORM_NON_WIN32
        strcpy(libTls->commonTls.errDesc, "unknown error");
#else // LLBC_TARGET_PLATFORM_WIN32
        strcpy_s(libTls->commonTls.errDesc,
                 __LLBC_ERROR_DESC_SIZE, "unknown error");
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

        return libTls->commonTls.errDesc;
    }
    else // Library error.
    {
        if (libTls->commonTls.customErrDesc[0] != '\0')
            return libTls->commonTls.customErrDesc;

#if LLBC_TARGET_PLATFORM_NON_WIN32
        strcpy(libTls->commonTls.errDesc,
               (__g_errDesc[noPart] ? __g_errDesc[noPart] : ""));
#else // LLBC_TARGET_PLATFORM_WIN32
        strcpy_s(libTls->commonTls.errDesc,
                 __LLBC_ERROR_DESC_SIZE, (__g_errDesc[noPart] ? __g_errDesc[noPart] : ""));
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

        return libTls->commonTls.errDesc;
    }
}

bool LLBC_HasSubErrorNo(int no)
{
    return !LLBC_ERROR_TYPE_IS_LIBRARY(no);
}

const char *LLBC_FormatLastError()
{
    return LLBC_StrError(LLBC_GetLastError());
}

int LLBC_GetSubErrorNo()
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (LIKELY(libTls))
        return libTls->commonTls.subErrNo;

    return 0;
}

void LLBC_SetSubErrorNo(int subErrorNo)
{
    __LLBC_LibTls *libTls = __LLBC_GetLibTls();
    if (LIKELY(libTls))
        libTls->commonTls.subErrNo = subErrorNo;
}

int LLBC_AddCustomErrno(int no, const char *desc)
{
    if (LLBC_GetErrnoCustomPart(no) != LLBC_ERROR_CUSTOM)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }
    else if (!desc)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    __LLBC_LockCustomErr();
    if (__g_customErrDesc.find(no) != __g_customErrDesc.end())
    {
        __LLBC_UnlockCustomErr();
        LLBC_SetLastError(LLBC_ERROR_EXIST);

        return LLBC_FAILED;
    }

    // Append custom error describe to dict.
    __g_customErrDesc.insert(std::make_pair(no, desc));

    __LLBC_UnlockCustomErr();

    return LLBC_OK;
}

int LLBC_RemoveCustomErrno(int no)
{
    if (LLBC_GetErrnoCustomPart(no) != LLBC_ERROR_CUSTOM)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    __LLBC_LockCustomErr();

    typedef std::map<int, LLBC_String>::iterator _Iter;
    _Iter it = __g_customErrDesc.find(no);

    const bool found = it != __g_customErrDesc.end();
    if (found)
        __g_customErrDesc.erase(it);

    __LLBC_UnlockCustomErr();

    if (!found)
    {
        LLBC_SetLastError(LLBC_ERROR_NOT_FOUND);
        return LLBC_FAILED;
    }

    return LLBC_OK;
}

int LLBC_RemoveAllCustomErrnos()
{
    __LLBC_LockCustomErr();
    __g_customErrDesc.clear();
    __LLBC_UnlockCustomErr();

    return LLBC_OK;
}

void __LLBC_InitErrors()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_init(&__g_customErrLock, nullptr);
#else // LLBC_TARGET_PLATFORM_WIN32
    ::InitializeCriticalSection(&__g_customErrLock);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

void __LLBC_DestroyErrors()
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    pthread_mutex_destroy(&__g_customErrLock);
#else // LLBC_TARGET_PLATFORM_WIN32
    ::DeleteCriticalSection(&__g_customErrLock);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

__LLBC_NS_END
