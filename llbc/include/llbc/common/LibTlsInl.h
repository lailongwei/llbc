/**
 * @file    LibTlsInl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/08/19
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_LIB_TLS_INL_H__
#define __LLBC_COM_LIB_TLS_INL_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"
#include "llbc/common/Config.h"
#include "llbc/common/BasicDataType.h"

__LLBC_NS_BEGIN

/**
 * \brief The Library TLS structure encapsulation.
 */
struct __LLBC_LibTls
{
    /* Common-Module TLS value. */
    struct  
    {
        /* error, sub-error, and error describe character pointer. */
        int errNo;
        int subErrNo;
        char errDesc[__LLBC_ERROR_DESC_SIZE];
#if LLBC_TARGET_PLATFORM_NON_WIN32
        char rtti[__LLBC_RTTI_BUF_SIZE];
#endif
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

        /* thread handle. */
        LLBC_Handle threadHandle;
        /* native thread handle, can use to OS dependency API. */
        LLBC_NativeThreadHandle nativeThreadHandle;

        /* Task pointer. */
        void *task;

        /* Timer scheduler. */
        void *timerScheduler;
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
};

/**
 * The Lib TLS handle create function.
 */
LLBC_EXTERN void __LLBC_CreateLibTls();

/**
 * The lib TLS handle destroy function.
 */
LLBC_EXTERN void __LLBC_DestroyLibTls();

/**
 * Get lib TLS value.
 * @return __LLBC_LibTls * - lib TLS value pointer.
 */
LLBC_EXTERN LLBC_EXPORT __LLBC_LibTls *__LLBC_GetLibTls();

/**
 * Reset lib TLS value.
 */
void __LLBC_ResetLibTls();

__LLBC_NS_END

#endif // !__LLBC_COM_LIB_TLS_INL_H__

