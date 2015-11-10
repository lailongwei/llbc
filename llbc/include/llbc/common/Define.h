/**
 * @file    Define.h
 * @author  Longwei Lai
 * @date    2013/03/31
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COM_DEFINE_H__
#define __LLBC_COM_DEFINE_H__

#include "llbc/common/PFConfig.h"

#include "llbc/common/Macro.h"

__LLBC_NS_BEGIN

/**
 * Common return values define.
 */
#define LLBC_RTN_OK     (0)
#define LLBC_RTN_FAILED (-1)

/**
 * carriage return / line feed.
 */
#define LLBC_CR_A         '\r'
#define LLBC_LF_A         '\n'
#define LLBC_CRLF_A       "\r\n"
#define LLBC_CR_W         L##'\r'
#define LLBC_LF_W         L##'\n'
#define LLBC_CRLF_W       L##"\r\n"
#define LLBC_CR           LLBC_TEXT('\r')
#define LLBC_LF           LLBC_TEXT('\n')
#define LLBC_CRLF         LLBC_TEXT("\r\n")

// slash('/') and backlash('\\') define.
#define LLBC_SLASH_A      '/'
#define LLBC_BACKLASH_A   '\\'
#define LLBC_SLASH_W      L##'/'
#define LLBC_BACKLASH_W   L##'\\'
#define LLBC_SLASH        LLBC_TEXT('/')
#define LLBC_BACKLASH     LLBC_TEXT('\\')

// Wait infinite macro define.
#if LLBC_TARGET_PLATFORM_NON_WIN32
#define LLBC_INFINITE    ((int)(0xffffffff))
#else // LLBC_TARGET_PLATFORM_WIN32
#define LLBC_INFINITE    INFINITE
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

__LLBC_NS_END

#endif // !__LLBC_COM_DEFINE_H__

