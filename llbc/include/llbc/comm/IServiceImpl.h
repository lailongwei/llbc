/**
 * @file    IServiceImpl.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/11/16
 * @version 1.0
 *
 * @brief
 */
#ifdef __LLBC_COMM_ISERVICE_H__

#include "llbc/comm/Packet.h"
#include "llbc/comm/PacketHeaderParts.h"

__LLBC_NS_BEGIN

template <typename T>
inline int LLBC_IService::Send(int sessionId, int opcode, const T &data, int status)
{
    return this->Send2<T>(sessionId, opcode, data, status, NULL);
}

template <typename T>
inline int LLBC_IService::Send(int svcId, int sessionId, int opcode, const T &data, int status)
{
    return this->Send2<T>(svcId, sessionId, opcode, data, status, NULL);
}

template <typename T>
inline int LLBC_IService::Send2(int sessionId, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Send2<T>(0, sessionId, opcode, data, status, parts);
}

template <typename T>
inline int LLBC_IService::Send2(int svcId, int sessionId, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts)
{
    LLBC_Packet *packet = LLBC_New(LLBC_Packet);
    packet->SetHeader(svcId, sessionId, opcode, status);
    if (parts)
    {
        parts->SetToPacket(*packet);
        LLBC_Delete(parts);
    }

    *packet <<data;

    return this->Send(packet);
}

template <typename T>
inline int LLBC_IService::Multicast(const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status)
{
    return this->Multicast2<T>(sessionIds, opcode, data, status, NULL);
}

template <typename T>
inline int LLBC_IService::Multicast(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status)
{
    return this->Multicast2<T>(svcId, sessionIds, opcode, data, status, NULL);
}

template <typename T>
inline int LLBC_IService::Multicast2(const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Multicast2(0, sessionIds, opcode, data, status, parts);
}

template <typename T>
inline int LLBC_IService::Multicast2(int svcId, const LLBC_SessionIdList &sessionIds, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts)
{
    LLBC_Stream stream;
    stream.Write(data);

    const void *bytes = stream.GetBuf();
    const size_t len = stream.GetPos();

    return this->Multicast2(svcId, sessionIds, opcode, bytes, len, status, parts);
}

template <typename T>
inline int LLBC_IService::Broadcast(int opcode, const T &data, int status)
{
    return this->Broadcast2<T>(opcode, data ,status, NULL);
}

template <typename T>
inline int LLBC_IService::Broadcast(int svcId, int opcode, const T &data, int status)
{
    return this->Broadcast2<T>(svcId, opcode, data, status, NULL);
}

template <typename T>
inline int LLBC_IService::Broadcast2(int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts)
{
    return this->Broadcast2<T>(0, opcode, data, status, parts);
}

template <typename T>
inline int LLBC_IService::Broadcast2(int svcId, int opcode, const T &data, int status, LLBC_PacketHeaderParts *parts)
{
    LLBC_Stream stream;
    stream.Write(data);

    const void *bytes = stream.GetBuf();
    const size_t len = stream.GetPos();

    return this->Broadcast2(svcId, opcode, bytes, len, status, parts);
}

template <typename ObjType>
inline int LLBC_IService::Subscribe(int opcode, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    LLBC_IDelegate1<LLBC_Packet &> *deleg = 
        new LLBC_Delegate1<ObjType, LLBC_Packet &>(obj, method);
    if (this->Subscribe(opcode, deleg) != LLBC_RTN_OK)
    {
        delete deleg;
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

template <typename ObjType>
inline int LLBC_IService::PreSubscribe(int opcode, ObjType *obj, void *(ObjType::*method)(LLBC_Packet &))
{
    LLBC_IDelegateEx<LLBC_Packet &> *deleg =
        new LLBC_DelegateEx<ObjType, LLBC_Packet &>(obj, method);
    if (this->PreSubscribe(opcode, deleg) != LLBC_RTN_OK)
    {
        delete deleg;
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

#if LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE
template <typename ObjType>
inline int LLBC_IService::UnifyPreSubscribe(ObjType *obj, void *(ObjType::*method)(LLBC_Packet &))
{
    LLBC_IDelegateEx<LLBC_Packet &> *deleg =
        new LLBC_DelegateEx<ObjType, LLBC_Packet &>(obj, method);
    if (this->UnifyPreSubscribe(deleg) != LLBC_RTN_OK)
    {
        delete deleg;
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_UNIFY_PRESUBSCRIBE

#if LLBC_CFG_COMM_ENABLE_STATUS_HANDLER
template <typename ObjType>
inline int LLBC_IService::SubscribeStatus(int opcode, int status, ObjType *obj, void (ObjType::*method)(LLBC_Packet &))
{
    LLBC_IDelegate1<LLBC_Packet &> *deleg =
        new LLBC_Delegate1<ObjType, LLBC_Packet &>(obj, method);
    if (this->SubscribeStatus(opcode, status, deleg) != LLBC_RTN_OK)
    {
        delete deleg;
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}
#endif // LLBC_CFG_COMM_ENABLE_STATUS_HANDLER

template <typename ObjType>
inline LLBC_ListenerStub LLBC_IService::SubscribeEvent(int event, ObjType *obj, void (ObjType::*method)(LLBC_Event *))
{
    LLBC_IDelegate1<LLBC_Event *> *deleg = 
        new LLBC_Delegate1<ObjType, LLBC_Event *>(obj, method);
    LLBC_ListenerStub stub = this->SubscribeEvent(event, deleg);
    if (stub.empty())
        delete deleg;

    return stub;
}

template <typename ObjType>
inline int LLBC_IService::Post(ObjType *obj, void (ObjType::*method)(LLBC_IService::This *svc))
{
    LLBC_IDelegate1<LLBC_IService::This *> *deleg = new LLBC_Delegate1<ObjType, LLBC_IService::This *>(obj, method);
    if (this->Post(deleg) != LLBC_RTN_OK)
    {
        delete deleg;
        return LLBC_RTN_FAILED;
    }

    return LLBC_RTN_OK;
}

__LLBC_NS_END

#endif // __LLBC_COMM_ISERVICE_H__

