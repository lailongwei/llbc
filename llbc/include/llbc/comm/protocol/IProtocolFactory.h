/**
 * @file    IProtocolFactory.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @version 1.0
 * @date    2018/02/07
 * 
 * @brief   
 */
#ifndef __LLBC_COMM_IPROTOCOL_FACTORY_H__
#define __LLBC_COMM_IPROTOCOL_FACTORY_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_IProtocol;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The protocol factory interface encapsulation.
 */
class LLBC_EXPORT LLBC_IProtocolFactory
{
public:
    virtual ~LLBC_IProtocolFactory() {  }

public:
    /**
     * Create specific layer protocol.
     * @return LLBC_IProtocol * - the protocol pointer.
     */
    virtual LLBC_IProtocol *Create(int layer) const = 0;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_IPROTOCOL_FACTORY_H__
