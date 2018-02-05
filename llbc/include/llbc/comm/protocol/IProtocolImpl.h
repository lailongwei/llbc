/**
 * @file    IProtocolImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/18
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_COMM_IPROTOCOL_H__

#include "llbc/comm/protocol/ProtocolLayer.h"

__LLBC_NS_BEGIN

LLBC_INLINE void LLBC_IProtocol::SetStack(LLBC_ProtocolStack *stack)
{
    _stack = stack;
}

LLBC_INLINE void LLBC_IProtocol::SetFilter(LLBC_IProtocolFilter *filter)
{
    _filter = filter;
}

LLBC_INLINE int LLBC_IProtocol::SetCoders(const Coders *coders)
{
    if (GetLayer() != LLBC_ProtocolLayer::CodecLayer)
    {
        LLBC_SetLastError(LLBC_ERROR_INVALID);
        return LLBC_FAILED;
    }

    _coders = coders;
    return LLBC_OK;
}

__LLBC_NS_END

#endif // __LLBC_COMM_IPROTOCOL_H__
