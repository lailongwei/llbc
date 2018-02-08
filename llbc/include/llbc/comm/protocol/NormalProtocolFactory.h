/**
 * @file    NormalProtocolFactory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2018/02/07
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_NORMAL_PROTOCOL_FACTORY_H__
#define __LLBC_COMM_NORMAL_PROTOCOL_FACTORY_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

#include "llbc/comm/protocol/IProtocolFactory.h"

__LLBC_NS_BEGIN

/**
 * \brief The llbc library normal protocol factory encapsulation.
 */
class LLBC_HIDDEN LLBC_NormalProtocolFactory : public LLBC_IProtocolFactory
{
public:
    /**
     * Create specific layer protocol.
     * @return LLBC_IProtocol * - the protocol pointer.
     */
    virtual LLBC_IProtocol *Create(int layer) const;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_NORMAL_PROTOCOL_FACTORY_H__
