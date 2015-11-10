/**
 * @file    ICoder.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/16
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_COMM_CODER_H__
#define __LLBC_COMM_CODER_H__

#include "llbc/common/Common.h"
#include "llbc/core/Core.h"
#include "llbc/objbase/ObjBase.h"

__LLBC_NS_BEGIN

/**
 * Pre-declare some classes.
 */
class LLBC_Packet;

__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The packet coder interface class encapsulation.
 */
class LLBC_ICoder
{
public:
    virtual ~LLBC_ICoder() {  }

public:
    /**
     * Encode pure virtual function, implement it to use encode packet data.
     */
    virtual void Encode(LLBC_Packet &packet) = 0;

    /**
     * Decode pure virtual function, implement it to use decode packet data.
     */
    virtual void Decode(LLBC_Packet &packet) = 0;
};

/**
 * \brief The packet coder factory interface class encapsulation.
 */
class LLBC_ICoderFactory
{
public:
    virtual ~LLBC_ICoderFactory() {  }

public:
    /**
     * Create coder.
     * @return LLBC_ICoder * - coder.
     */
    virtual LLBC_ICoder *Create() const = 0;
};

__LLBC_NS_END

#endif // !__LLBC_COMM_CODER_H__
