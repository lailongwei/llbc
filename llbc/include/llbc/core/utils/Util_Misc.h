/**
 * @file    Util_Misc.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/05
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_UTILS_UTIL_MISC_H__
#define __LLBC_CORE_UTILS_UTIL_MISC_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The invoke guard helper class encapsulation.
 */
class LLBC_InvokeGuard
{
public:
    /**
     * Create function type invoke guard.
     * @param[in] func - the guard invoke function pointer.
     * @param[in] data - the call data, default is NULL.
     */
    explicit LLBC_InvokeGuard(LLBC_GuardFunc func, void *data = NULL);
    
    /**
     * Create method type invoke guard.
     * @param[in] obj  - the class instance pointer.
     * @param[in] meth - the guard invoke method pointer
     * @param[in] data - the call data, default is NULL.
     */
    template <typename Object>
    LLBC_InvokeGuard(Object *obj, void (Object::*meth)(void *), void *data = NULL);

    virtual ~LLBC_InvokeGuard();

private:
    LLBC_DISABLE_ASSIGNMENT(LLBC_InvokeGuard);

private:
    LLBC_GuardFunc _func;
    LLBC_IDelegate1<void *> *_meth;

    void *_data;
};

__LLBC_NS_END

#include "llbc/core/utils/Util_MiscImpl.h"

#endif // !__LLBC_CORE_UTILS_UTIL_MISC_H__
