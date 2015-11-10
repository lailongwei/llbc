/**
 * @file    ObjectMacro.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/02/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_OBJBASE_OBJECT_MACRO_H__
#define __LLBC_OBJBASE_OBJECT_MACRO_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"

__LLBC_NS_BEGIN

// Safe release macro define.
#define LLBC_SAFE_RELEASE(o) \
    do {                     \
        if (LIKELY(o)) {     \
            o->Release();    \
        }                    \
    } while(0)               \

// Safe release and set object pointer to null macro define.
#define LLBC_SAFE_RELEASE_NULL(o) \
    do {                          \
        if (LIKELY(o)) {          \
            o->Release();         \
            o = NULL;             \
        }                         \
    } while(0)                    \

__LLBC_NS_END

#endif // !__LLBC_OBJBASE_OBJECT_MACRO_H__
