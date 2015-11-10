/**
 * @file    IHolder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_ENTITY_IHOLDER_H__
#define __LLBC_CORE_ENTITY_IHOLDER_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
 * \brief The holder interface class encapsulation.
 */
class LLBC_EXPORT LLBC_IHolder
{
public:
    virtual ~LLBC_IHolder() {  }

public:
    /**
     * Get holder value.
     * @return [in] void * - value.
     */
    virtual void *GetValue() const = 0;
};

__LLBC_NS_END

#endif // !__LLBC_CORE_ENTITY_IHOLDER_H__
