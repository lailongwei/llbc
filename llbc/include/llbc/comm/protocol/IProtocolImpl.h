/**
 * @file    IProtocolImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/18
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_COMM_IPROTOCOL_H__

#include "llbc/comm/protocol/RawProtocol.h"
#include "llbc/comm/protocol/PacketProtocol.h"
#include "llbc/comm/protocol/CompressProtocol.h"
#include "llbc/comm/protocol/CodecProtocol.h"

__LLBC_INTERNAL_NS_BEGIN

inline static LLBC_NS LLBC_IProtocol *__SetProtocol(LLBC_NS LLBC_IProtocol *proto, LLBC_NS LLBC_IProtocolFilter *filter)
{
    if (filter)
        proto->SetFilter(filter);

    return proto;
}

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

inline void LLBC_IProtocol::SetStack(LLBC_ProtocolStack *stack)
{
    _stack = stack;
}

inline void LLBC_IProtocol::SetFilter(LLBC_IProtocolFilter *filter)
{
    _filter = filter;
}

template <>
inline LLBC_IProtocol *LLBC_IProtocol::Create<LLBC_RawProtocol>(LLBC_IProtocolFilter *filter)
{
    return LLBC_INL_NS __SetProtocol(LLBC_New(LLBC_RawProtocol), filter);
}

template <>
inline LLBC_IProtocol *LLBC_IProtocol::Create<LLBC_PacketProtocol>(LLBC_IProtocolFilter *filter)
{
    return LLBC_INL_NS __SetProtocol(LLBC_New(LLBC_PacketProtocol), filter);
}

template <>
inline LLBC_IProtocol *LLBC_IProtocol::Create<LLBC_CompressProtocol>(LLBC_IProtocolFilter *filter)
{
    return LLBC_INL_NS __SetProtocol(LLBC_New(LLBC_CompressProtocol), filter);
}

template <>
inline LLBC_IProtocol *LLBC_IProtocol::Create<LLBC_CodecProtocol>(LLBC_IProtocolFilter *filter)
{
    return LLBC_INL_NS __SetProtocol(LLBC_New(LLBC_CodecProtocol), filter);
}

__LLBC_NS_END

#endif // __LLBC_COMM_IPROTOCOL_H__
