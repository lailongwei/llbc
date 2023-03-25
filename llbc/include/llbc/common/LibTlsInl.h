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
#include "llbc/common/Config.h"
#include "llbc/common/BasicDataType.h"

__LLBC_NS_BEGIN

/**
 * \brief The Library TLS structure encapsulation.
 */
struct LLBC_EXPORT __LLBC_LibTls
{
    /* Common-Module TLS value. */
    struct
    {
        /* error, sub-error, and error describe character pointer. */
        int errNo;
        int subErrNo;
        char errDesc[__LLBC_ERROR_DESC_SIZE];
        char customErrDesc[__LLBC_ERROR_DESC_SIZE];
        char clibErrFmtBuf[__LLBC_CLIB_ERROR_FORMAT_BUF_SIZE];
        char rtti[__LLBC_RTTI_BUF_SIZE];
    } commonTls;

    /* Core-Module TLS value. */
    struct
    {
        /* llbc created thread flag. */
        bool llbcThread;
        /* llbc entry thread flag. */
        bool entryThread;

        /* need init WinSock library, only available when entryThread is true */
        bool needInitWinSock;

        /* logger format buffer(used for vsnprintf()) */
        char loggerFmtBuf[LLBC_CFG_LOG_FORMAT_BUF_SIZE + 1];

        /* native thread id, can use to OS dependency APIs. */
        LLBC_ThreadId threadId;
        /* thread id/handle. */
        LLBC_Handle threadHandle;
        /* native thread handle, can use to OS dependency APIs. */
        LLBC_NativeThreadHandle nativeThreadHandle;
        /* thread group handle. */
        LLBC_Handle threadGroupHandle;

        /* Task pointer. */
        void *task;

        /* Timer scheduler. */
        void *timerScheduler;

        /* ObjectPool pointers. */
        void *safetyObjectPool;
        void *unsafetyObjectPool;

        /* symbol about data. */
        #if LLBC_CFG_OS_IMPL_SYMBOL
        struct
        {
            #if LLBC_TARGET_PLATFORM_WIN32
            /* Win32 spec: the win32 symbol structure. */
            SYMBOL_INFO *win32Symbol;
            /* Win32 spec: the image helper structure, use for store symbol file name and line number. */
            IMAGEHLP_LINE64 win32ImgHelpLine64;
            #endif // LLBC_TARGET_PLATFORM_WIN32

            /* The call stack. */
            void *stack[LLBC_CFG_OS_SYMBOL_MAX_CAPTURE_FRAMES];
        } symbol;
        #endif // LLBC_CFG_OS_IMPL_SYMBOL
    } coreTls;

    /* ObjBase-Module TLS valus. */
    struct
    {
        /* Auto-Release pool stack. */
        void *poolStack;
    } objbaseTls;

    /* Communication-Module TLS value. */
    struct
    {
        /* Services pointer. */
        void *services[LLBC_CFG_COMM_PER_THREAD_DRIVE_MAX_SVC_COUNT + 1];
    } commTls;

    __LLBC_LibTls();
    ~__LLBC_LibTls();
};

/**
 * The entry-thread lib tls variable.
 */
extern LLBC_EXPORT __LLBC_LibTls *__LLBC_EntryThreadLibTls;

/**
 * The Lib TLS handle create function.
 */
LLBC_HIDDEN void __LLBC_CreateLibTls();

/**
 * The lib TLS handle destroy function.
 */
LLBC_HIDDEN void __LLBC_DestroyLibTls();

/**
 * Get lib TLS value.
 * @return __LLBC_LibTls * - lib TLS value pointer.
 */
LLBC_EXPORT __LLBC_LibTls *__LLBC_GetLibTls();

/**
 * Get entry-thread lib TLS value.
 * @return __LLBC_LibTls * - entry-thread lib TLS value pointer.
 */
LLBC_EXPORT __LLBC_LibTls *__LLBC_GetEntryThreadLibTls();

/**
 * Reset lib TLS value.
 */
void __LLBC_ResetLibTls();

__LLBC_NS_END
