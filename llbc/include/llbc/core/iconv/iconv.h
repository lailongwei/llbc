/**
 * @file    iconv.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/09/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_ICONV_ICONV_H__
#define __LLBC_CORE_ICONV_ICONV_H__

#include "llbc/common/Common.h"

#if LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

typedef void* iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode);

int iconv_close(iconv_t cd);

size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);

__LLBC_NS_END

#endif // LLBC_TARGET_PLATFORM_WIN32

#endif// !__LLBC_CORE_ICONV_ICONV_H__