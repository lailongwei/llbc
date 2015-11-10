/**
 * @file    Holder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/13
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_CORE_ENTITY_HOLDER_H__
#define __LLBC_CORE_ENTITY_HOLDER_H__

#include "llbc/common/Common.h"

#include "llbc/core/entity/IHolder.h"

__LLBC_NS_BEGIN

/**
 * \brief The holder class encapsulation.
 */
template <typename ValueType>
class LLBC_Holder : public LLBC_IHolder
{
public:
    explicit LLBC_Holder(ValueType *value);
    virtual ~LLBC_Holder();

public:
    /**
     * Get holder value.
     * @return [in] void * - value.
     */
    virtual void *GetValue() const;

    /**
     * Disable assignment.
     */
    LLBC_DISABLE_ASSIGNMENT(LLBC_Holder);

private:
    ValueType *_value;
};

__LLBC_NS_END

#include "llbc/core/entity/HolderImpl.h"

#endif // !__LLBC_CORE_ENTITY_HOLDER_H__
